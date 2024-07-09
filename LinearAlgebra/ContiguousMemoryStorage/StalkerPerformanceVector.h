//
// Created by hal9000 on 7/6/24.
//

#ifndef STALKER_STALKERPERFORMANCEVECTOR_H
#define STALKER_STALKERPERFORMANCEVECTOR_H
#include <cstdlib>
#include <immintrin.h>
#include <stdexcept>
#include <unordered_map>
#include <functional>
#include <typeindex>
#include <cstring>
#include "../../Operations/AVX_MemoryManagement.h"
#include "../../Operations/AVX_Operations.h"
#include "ContiguousMemoryIterator.h"

template <typename T, unsigned unrollFactor>
class StalkerPerformanceVector {
    
    static_assert( AVX_MemoryManagement::checkInputType<T>(), "Invalid data type. Supported types are float, double, int, short, and unsigned.");
    
    using Traits = AVX_MemoryTraits<T, unrollFactor>;
    using avxRegister = typename Traits::AVXRegisterType;
    using avxData = typename Traits::AVXRegisterType*;
    
public:
    explicit StalkerPerformanceVector(unsigned size, T value = 0)
            : _avxTraits() {
        _size = size;
        _alignment = 64;
        _sizeInCacheLines = (size * sizeof(T) + 63) / 64;
        _avxRegisterSize = _avxTraits.AVXRegisterSize;
        _sizeInAVXRegisters = (size + _avxRegisterSize - 1) / _avxRegisterSize;
        _data = AVX_MemoryManagement::allocate<T>(size, _alignment);
        std::fill(_data, _data + size, value);
    }


    StalkerPerformanceVector(T* data, unsigned size) {
        _size = size;
        _alignment = 64;
        _sizeInCacheLines = (size * sizeof(T) + 63) / 64; // Adding 63 ensures any remainder gets an extra cache line
        _avxRegisterSize = Traits::AVXRegisterSize;
        _sizeInAVXRegisters = (size + _avxRegisterSize - 1) / _avxRegisterSize; // Adding register size - 1 ensures any remainder gets an extra register
        _data = AVX_MemoryManagement::allocate<T>(size, _alignment);
        std::memcpy(_data, data, size * sizeof(T));
    }


    // Copy constructor
    StalkerPerformanceVector(const StalkerPerformanceVector& other)
            : _size(other._size), _alignment(other._alignment),
              _sizeInCacheLines(other._sizeInCacheLines),
              _avxRegisterSize(other._avxRegisterSize),
              _sizeInAVXRegisters(other._sizeInAVXRegisters) {
        _data = AVX_MemoryManagement::allocate<T>(_size, _alignment);
        std::memcpy(_data, other._data, _size * sizeof(T));
    }

    // Copy assignment operator
    StalkerPerformanceVector& operator=(const StalkerPerformanceVector& other) {
        if (this != &other) {
            if (_size != other._size) {
                T* newData = AVX_MemoryManagement::allocate<T>(other._size, other._alignment);
                std::memcpy(newData, other._data, other._size * sizeof(T));
                std::free(_data);
                _data = newData;
                _size = other._size;
                _alignment = other._alignment;
                _sizeInCacheLines = other._sizeInCacheLines;
                _avxRegisterSize = other._avxRegisterSize;
                _sizeInAVXRegisters = other._sizeInAVXRegisters;
            } else {
                std::memcpy(_data, other._data, _size * sizeof(T));
            }
        }
        return *this;
    }

    // Move constructor
    StalkerPerformanceVector(StalkerPerformanceVector&& other) noexcept
            : _size(other._size), _alignment(other._alignment),
              _sizeInCacheLines(other._sizeInCacheLines),
              _avxRegisterSize(other._avxRegisterSize),
              _sizeInAVXRegisters(other._sizeInAVXRegisters),
              _data(other._data) {
        other._data = nullptr;
        other._size = 0;
        other._sizeInCacheLines = 0;
        other._avxRegisterSize = 0;
        other._sizeInAVXRegisters = 0;
    }

    // Move assignment operator
    StalkerPerformanceVector& operator=(StalkerPerformanceVector&& other) noexcept {
        if (this != &other) {
            std::free(_data);
            _data = other._data;
            _size = other._size;
            _alignment = other._alignment;
            _sizeInCacheLines = other._sizeInCacheLines;
            _avxRegisterSize = other._avxRegisterSize;
            _sizeInAVXRegisters = other._sizeInAVXRegisters;

            other._data = nullptr;
            other._size = 0;
            other._sizeInCacheLines = 0;
            other._avxRegisterSize = 0;
            other._sizeInAVXRegisters = 0;
        }
        return *this;
    }

    // Destructor
    ~StalkerPerformanceVector() {
        std::free(_data);
        _data = nullptr;
    }

    inline T& operator[](unsigned index) {
        return _data[index];
    }

    inline const T& operator[](unsigned index) const {
        return _data[index];
    }
    
    //Iterators
    inline T* begin() {
        return _data;
    }

    inline T* end() {
        return _data + _size;
    }

    inline const T* begin() const {
        return _data;
    }

    inline const T* end() const {
        return _data + _size;
    }
    
    std::pair<Iterator<T>, Iterator<T>> range(unsigned start, unsigned end) {
        if (start >= _size || end > _size || start >= end)
            throw std::out_of_range("Invalid range.");
        return std::make_pair(Iterator<T>(_data + start), Iterator<T>(_data + end));
    }

    inline T* data() {
        return _data;
    }
    const inline T* data() const {
        return _data;
    }

    [[nodiscard]] inline unsigned size() const{
        return _size;
    }
    

    [[nodiscard]] inline unsigned sizeInCacheLines() const{
        return _sizeInCacheLines;
    }

    [[nodiscard]] inline unsigned sizeInAVXRegisters() const{
        return _sizeInAVXRegisters;
    }

    [[nodiscard]] inline unsigned avxRegisterSize() const{
        return _avxRegisterSize;
    }

    [[nodiscard]] inline unsigned alignment() const{
        return _alignment;
    }



    void deepCopy(const StalkerPerformanceVector& other) {
        //AVX_Operations<T, unrollFactor>::deepcopy(_data, other._data, _size, false);
        auto manager = CPU_Manager();
        auto threads = manager.getHyperThreadCores(2);
        AVX_Operations<T, unrollFactor>::deepcopy(_data, other._data, _size, 2, manager, true);
    }
    
    void deepCopyMultithreaded(const StalkerPerformanceVector& other) {
//        auto cpu = CPU_Manager();
//        auto threads = cpu.getHyperThreadCores(2);
//        AVX_Operations<T, unrollFactor>::deepcopy(_data, other._data, _size, 2, true);
    }

    void setValue(T value) {
        AVX_Operations<T, unrollFactor>::setValue(_data, value, _size);
    }
    
    bool areEqual(const StalkerPerformanceVector& other) {
//        for (unsigned i = 0; i < _size; ++i) {
//            if (_data[i] != other._data[i]) {
//                return false;
//            }
//        }
        return AVX_Operations<T, unrollFactor>::areEqual(_data, other._data, _size);
    }

private:
    T* _data;
    unsigned _size;
    unsigned _sizeInCacheLines;
    unsigned _avxRegisterSize;
    unsigned _sizeInAVXRegisters;
    unsigned _alignment;
    AVX_MemoryTraits<T, unrollFactor> _avxTraits;
};

#endif //STALKER_STALKERPERFORMANCEVECTOR_H
