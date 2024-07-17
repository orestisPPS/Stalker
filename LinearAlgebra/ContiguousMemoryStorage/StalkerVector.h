//
// Created by hal9000 on 7/6/24.
//

#ifndef STALKER_STALKERVECTOR_H
#define STALKER_STALKERVECTOR_H

#include <cstring>
#include <valarray>
#include "../../Operations/AVX_MemoryManagement.h"
#include "../../Operations/AVX_Operations.h"
#include "ContiguousMemoryIterator.h"
#include "../../Operations/AVX_MathTraits.h"


enum UnrollFactor {
    UNROLL_1,
    UNROLL_2,
    UNROLL_4,
    UNROLL_8,
    UNROLL_16,
    UNROLL_32,
    UNROLL_64,
};

static constexpr unsigned int getUnrollFactor(UnrollFactor unrollFactor) {
    switch (unrollFactor) {
        case UNROLL_1:
            return 1;
        case UNROLL_2:
            return 2;
        case UNROLL_4:
            return 4;
        case UNROLL_8:
            return 8;
        case UNROLL_16:
            return 16;
        case UNROLL_32:
            return 32;
        case UNROLL_64:
            return 64;
        default:
            return 1;
    }
}

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


    void fill(T value) {
        AVX_Operations<T, unrollFactor>::setValue(_data, value, _size, _numCores, _manager, true);
    }
    
    void scale(T value) {
        AVX_Operations<T, unrollFactor>::scale(_data, value, _size, _numCores, _manager, true);
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
    
    T sum () {
        return AVX_Operations<T, unrollFactor>::sum(_data, _size, _numCores, _manager);
    }
    
    T dotProduct(const StalkerVector& other) {
        return AVX_Operations<T, unrollFactor>::dotProduct(_data, other._data, _size, _numCores, _manager);
    }
    
    double magnitude() {
        if constexpr (std::is_same<T, int>::value || std::is_same<T, short>::value ||
                      std::is_same<T, unsigned>::value || std::is_same<T, float>::value)
            return sqrt(static_cast<double>(dotProduct(*this)));
        else if constexpr (std::is_same<T, double>::value)
            return sqrt(dotProduct(*this));
    }
    
    double average() {
        return static_cast<double>(sum()) / _size;
    }
    


    void printVertically(const std::string& name = " "){
        std::cout << " name = [ " << std::endl;
        for (auto &value: *_data){
            std::cout << value << std::endl;
        }
        std::cout << " ] " << std::endl;
    }

    void printVerticallyWithIndex(const std::string& name = " "){
        std::cout << name << " = [ " << std::endl;
        for (unsigned i = 0; i < _size - 1; ++i){
            std::cout << i << " : " << (*_data)[i] << std::endl;
        }
        std::cout << _size - 1 << " : " << (*_data)[_size - 1] << " ] " << std::endl;
    }


    void printHorizontally(const std::string& name = " "){
        std::ostream os (std::cout.rdbuf());
        os << name << " = [ ";
        for (const auto &value: *_data){
            os << value << " ";
        }
        os << " ] " << "\n" << std::endl;
    }

    void printHorizontallyWithIndex(const std::string& name = " "){
        std::ostream os (std::cout.rdbuf());
        os << name << " = [ ";
        for (unsigned i = 0; i < _size; ++i){
            os << i << " : " << (*_data)[i] << " ";
        }
        os << " ] " << "\n" << std::endl;
    }


    void exportCSV(const std::string& filePath, const std::string& fileName) const {
        std::ofstream file;
        std::string fullPath = filePath + "/" + fileName + ".csv";
        std::cout << "Exporting to " << fullPath << std::endl;

        file.open(fullPath);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file.");
        }

        for (unsigned i = 0; i < _size; ++i) {
            file << (*_data)[i] << '\n';  // Write each value in a new line
        }

        file.close();
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
