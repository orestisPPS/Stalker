//
// Created by hal9000 on 7/6/24.
//

#ifndef STALKER_STALKERVECTOR_H
#define STALKER_STALKERVECTOR_H
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
#include "../../Operations/AVX_MathTraits.h"

struct VectorThreadingConfig{
    unsigned numCores = 2;
    bool enableHyperThreading = false;
    WorkDistributionPolicy workDistributionPolicy;
};

template <typename T, unsigned unrollFactor>
class StalkerVector {
    
    static_assert( AVX_MemoryManagement::checkInputType<T>(), "Invalid data type. Supported types are float, double, int, short, and unsigned.");
    
public:

    explicit StalkerVector(unsigned size, CPU_Manager &manager)
            : _avxMemoryTraits(), _manager(manager)  {
        _size = size;
        _alignment = 64;
        _sizeInCacheLines = (size * sizeof(T) + 63) / 64;
        _data = AVX_MemoryManagement::allocate<T>(size, _alignment);
        std::fill(_data, _data + size, 0);
    }

    explicit StalkerVector(unsigned size, T value , CPU_Manager &manager)
            : _avxMemoryTraits(), _manager(manager) {
        _size = size;
        _alignment = 64;
        _sizeInCacheLines = (size * sizeof(T) + 63) / 64;
        _data = AVX_MemoryManagement::allocate<T>(size, _alignment);
        std::fill(_data, _data + size, value);
    }
    
    StalkerVector(T* data, unsigned size, CPU_Manager &manager)
            : _avxMemoryTraits(), _manager(manager)  {
        _size = size;
        _alignment = 64;
        _sizeInCacheLines = (size * sizeof(T) + 63) / 64; // Adding 63 ensures any remainder gets an extra cache line_sizeInAVXRegisters = (size + _avxRegisterSize - 1) / _avxRegisterSize; // Adding register size - 1 ensures any remainder gets an extra register
        _data = AVX_MemoryManagement::allocate<T>(size, _alignment);
        std::memcpy(_data, data, size * sizeof(T));
    }


    // Copy constructor
    StalkerVector(const StalkerVector& other)
        : _size(other._size),
          _alignment(other._alignment),
          _sizeInCacheLines(other._sizeInCacheLines),
          _avxMemoryTraits(other._avxMemoryTraits),
          _manager(other._manager){
        _data = AVX_MemoryManagement::allocate<T>(_size, _alignment);
        std::memcpy(_data, other._data, _size * sizeof(T));
    }

    // Copy assignment operator
    StalkerVector& operator=(const StalkerVector& other) {
        if (this != &other) {
            if (_size != other._size) {
                T* newData = AVX_MemoryManagement::allocate<T>(other._size, other._alignment);
                std::memcpy(newData, other._data, other._size * sizeof(T));
                std::free(_data);
                _data = newData;
                _size = other._size;
                _alignment = other._alignment;
                _sizeInCacheLines = other._sizeInCacheLines;
            } else {
                std::memcpy(_data, other._data, _size * sizeof(T));
            }
            _manager = other._manager;
        }
        return *this;
    }

    // Move constructor
    StalkerVector(StalkerVector&& other) noexcept
            : _size(other._size), _alignment(other._alignment),
              _sizeInCacheLines(other._sizeInCacheLines),
              _data(other._data),
              _manager(other._manager){
        other._data = nullptr;
        other._size = 0;
        other._sizeInCacheLines = 0;
    }

    // Move assignment operator
    StalkerVector& operator=(StalkerVector&& other) noexcept {
        if (this != &other) {
            std::free(_data);
            _data = other._data;
            _size = other._size;
            _alignment = other._alignment;
            _sizeInCacheLines = other._sizeInCacheLines;
            _manager = other._manager;

            other._data = nullptr;
            other._size = 0;
            other._sizeInCacheLines = 0;
        }
        return *this;
    }

    // Destructor
    ~StalkerVector() {
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

    
    [[nodiscard]] inline unsigned getAVXRegisterSize() const{
        return _avxMemoryTraits.AVXRegisterSize;
    }
    
    [[nodiscard]] inline unsigned alignment() const{
        return _alignment;
    }
    
    void setAvailableCores(unsigned numCores) {
        _numCores = numCores;
    }

    void copy(const StalkerVector& other) {
        AVX_Operations<T, unrollFactor>::copy(_data, other._data, _size, _numCores, _manager, true);
    }


    void setValue(T value) {
        AVX_Operations<T, unrollFactor>::setValue(_data, value, _size, _numCores, _manager, true);
    }
    
    bool areEqual(const StalkerVector& other) {
        return AVX_Operations<T, unrollFactor>::areEqual(_data, other._data, _size, _numCores, _manager);
    }
    
    void add(const StalkerVector& other, StalkerVector& result, T scale1, T scale2) {
        AVX_Operations<T, unrollFactor>::add(_data, other._data, result._data, scale1, scale2, _size, _numCores, _manager, true);
    }
    
    void subtract(const StalkerVector& other, StalkerVector& result, T scale1, T scale2) {
        AVX_Operations<T, unrollFactor>::subtract(_data, other._data, result._data, scale1, scale2, _size, _numCores, _manager, true);
    }
    
    void multiply(const StalkerVector& other, StalkerVector& result, T scale1, T scale2) {
        AVX_Operations<T, unrollFactor>::multiply(_data, other._data, result._data, scale1, scale2, _size, _numCores, _manager, true);
    }
    
//    template<unsigned powerValue>
//    void raisePower(){
//        AVX_Operations<T, unrollFactor>::power<powerValue>(_data, _size, _numCores, _manager);
//    }

private:
    T* _data;
    unsigned _size;
    unsigned _sizeInCacheLines;
    unsigned _alignment;
    AVX_MemoryTraits<T, unrollFactor> _avxMemoryTraits;
    CPU_Manager &_manager;
    unsigned _numCores = 2;
};



#endif //STALKER_STALKERVECTOR_H
