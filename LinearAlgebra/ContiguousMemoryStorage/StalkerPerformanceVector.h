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
#include "ContiguousMemoryIterator.h"

template <typename T, unsigned unrollFactor>
class StalkerPerformanceVector {
    static_assert( AVX_MemoryManagement::checkInputType<T>(), "Invalid data type. Supported types are float, double, int, short, and unsigned.");
    using Traits = AVX_Traits<T, unrollFactor>;
    using AVXRegister = typename Traits::AVXRegister;
public:
    explicit StalkerPerformanceVector(unsigned size, T value = 0){
        _size = size;
        _alignment = 64;
        _sizeInCacheLines = (size * sizeof(T) + 63) / 64;
        _AVX2RegisterSize = Traits::AVXRegisterSize;
        _sizeInAVXRegisters = (size + _AVX2RegisterSize - 1) / _AVX2RegisterSize;
        _data = AVX_MemoryManagement::allocate<T>(size, _alignment);
        std::fill(_data, _data + size, value);
    }

    StalkerPerformanceVector(T* data, unsigned size) {
        _size = size;
        _alignment = 64;
        _sizeInCacheLines = (size * sizeof(T) + 63) / 64; // Adding 63 ensures any remainder gets an extra cache line
        _AVX2RegisterSize = Traits::AVXRegisterSize;
        _sizeInAVXRegisters = (size + _AVX2RegisterSize - 1) / _AVX2RegisterSize; // Adding register size - 1 ensures any remainder gets an extra register
        _data = AVX_MemoryManagement::allocate<T>(size, _alignment);
        std::memcpy(_data, data, size * sizeof(T));
    }


    // Copy constructor
    StalkerPerformanceVector(const StalkerPerformanceVector& other)
            : _size(other._size), _alignment(other._alignment),
              _sizeInCacheLines(other._sizeInCacheLines),
              _AVX2RegisterSize(other._AVX2RegisterSize),
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
                _AVX2RegisterSize = other._AVX2RegisterSize;
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
              _AVX2RegisterSize(other._AVX2RegisterSize),
              _sizeInAVXRegisters(other._sizeInAVXRegisters),
              _data(other._data) {
        other._data = nullptr;
        other._size = 0;
        other._sizeInCacheLines = 0;
        other._AVX2RegisterSize = 0;
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
            _AVX2RegisterSize = other._AVX2RegisterSize;
            _sizeInAVXRegisters = other._sizeInAVXRegisters;

            other._data = nullptr;
            other._size = 0;
            other._sizeInCacheLines = 0;
            other._AVX2RegisterSize = 0;
            other._sizeInAVXRegisters = 0;
        }
        return *this;
    }

    // Destructor
    ~StalkerPerformanceVector() {
        std::free(_data);
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

    [[nodiscard]] inline unsigned AVX2RegisterSize() const{
        return _AVX2RegisterSize;
    }

    [[nodiscard]] inline unsigned alignment() const{
        return _alignment;
    }


private:
    T* _data;
    AVXRegister _avxData[unrollFactor];
    unsigned _size;
    unsigned _sizeInCacheLines;
    unsigned _AVX2RegisterSize;
    unsigned _sizeInAVXRegisters;
    unsigned _alignment;
    AVX_Traits<T, unrollFactor> _AVXTraits;



};

#endif //STALKER_STALKERPERFORMANCEVECTOR_H
