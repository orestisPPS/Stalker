//
// Created by hal9000 on 6/7/24.
//

#ifndef STALKER_AVX_MEMORYMANAGEMENT_H
#define STALKER_AVX_MEMORYMANAGEMENT_H

// AVX register sizes in terms of elements per register
#define DOUBLE_AVX_REGISTER_SIZE 4     // 256 bits / 64 bits per double
#define FLOAT_AVX_REGISTER_SIZE 8      // 256 bits / 32 bits per float
#define INT_AVX_REGISTER_SIZE 8        // 256 bits / 32 bits per int
#define SHORT_AVX_REGISTER_SIZE 16     // 256 bits / 16 bits per short
#define UNSIGNED_AVX_REGISTER_SIZE 8   // 256 bits / 32 bits per unsigned int

#include <cstdlib>
#include <immintrin.h>
#include <stdexcept>
#include <unordered_map>
#include <functional>
#include <typeindex>

template<typename T, unsigned int unrollFactor>
struct AVX_Traits;

// Float Specialization
template<unsigned int unrollFactor>
struct AVX_Traits<float, unrollFactor> {
    using AVXRegister = __m256;
    using DataType = float;
    static constexpr unsigned AVXRegisterSize = FLOAT_AVX_REGISTER_SIZE;
    static constexpr unsigned elementsPerCacheLine = 64 / sizeof(float);
    static constexpr unsigned UnrollFactor = unrollFactor;
    AVXRegister avxArray[UnrollFactor];
    
    constexpr inline AVXRegister* getAVXRegister() {
        return avxArray;
    }
    

    static constexpr inline void loadAVXRegister(const DataType* source, AVXRegister* destination) {
        _loadAVXRegister<UnrollFactor>(source, destination);
    }

    static constexpr inline void storeAVXRegisterTemporal(const AVXRegister* source, DataType* destination) {
        _storeAVXRegisterTemporal<UnrollFactor>(source, destination);
    }

    static constexpr inline void storeAVXRegisterNonTemporal(const AVXRegister* source, DataType* destination) {
        _storeAVXRegisterNonTemporal<UnrollFactor>(source, destination);
    }

    static constexpr inline void setZeroAVXRegister(AVXRegister* destination) {
        _setZeroAVXRegister<UnrollFactor>(destination);
    }

    static constexpr inline void setValue(AVXRegister* destination, float value) {
        _setValue<UnrollFactor>(destination, value);
    }

private:
    template <unsigned iUnroll>
    static constexpr inline void _loadAVXRegister(const DataType* source, AVXRegister* destination) {
        if constexpr (iUnroll > 0) {
            *(destination + iUnroll - 1) = _mm256_load_ps(source + (iUnroll - 1) * FLOAT_AVX_REGISTER_SIZE);
            _loadAVXRegister<iUnroll - 1>(source, destination);
        }
    }

    template <unsigned iUnroll>
    static constexpr inline void _storeAVXRegisterTemporal(const AVXRegister* source, DataType* destination) {
        if constexpr (iUnroll > 0) {
            _mm256_store_ps(destination + (iUnroll - 1) * FLOAT_AVX_REGISTER_SIZE, *(source + iUnroll - 1));
            _storeAVXRegisterTemporal<iUnroll - 1>(source, destination);
        }
    }

    template <unsigned iUnroll>
    static constexpr inline void _storeAVXRegisterNonTemporal(const AVXRegister* source, DataType* destination) {
        if constexpr (iUnroll > 0) {
            _mm256_stream_ps(destination + (iUnroll - 1) * FLOAT_AVX_REGISTER_SIZE, *(source + iUnroll - 1));
            _storeAVXRegisterNonTemporal<iUnroll - 1>(source, destination);
        }
    }

    template <unsigned iUnroll>
    static constexpr inline void _setZeroAVXRegister(AVXRegister* destination) {
        if constexpr (iUnroll > 0) {
            *(destination + iUnroll - 1) = _mm256_setzero_ps();
            _setZeroAVXRegister<iUnroll - 1>(destination);
        }
    }

    template <unsigned iUnroll>
    static constexpr inline void _setValue(AVXRegister* destination, float value) {
        if constexpr (iUnroll > 0) {
            *(destination + iUnroll - 1) = _mm256_set1_ps(value);
            _setValue<iUnroll - 1>(destination, value);
        }
    }
};

// Double Specialization
template<unsigned int unrollFactor>
struct AVX_Traits<double, unrollFactor> {
    using AVXRegister = __m256d;
    using DataType = double;
    static constexpr unsigned AVXRegisterSize = DOUBLE_AVX_REGISTER_SIZE;
    static constexpr unsigned elementsPerCacheLine = 64 / sizeof(double);
    static constexpr unsigned UnrollFactor = unrollFactor;
    AVXRegister avxArray[UnrollFactor];
    
    
    constexpr inline AVXRegister* getAVXRegister() {
        return avxArray;
    }

    static constexpr inline void loadAVXRegister(const DataType* source, AVXRegister* destination) {
        _loadAVXRegister<UnrollFactor>(source, destination);
    }

    static constexpr inline void storeAVXRegisterTemporal(const AVXRegister* source, DataType* destination) {
        _storeAVXRegisterTemporal<UnrollFactor>(source, destination);
    }

    static constexpr inline void storeAVXRegisterNonTemporal(const AVXRegister* source, DataType* destination) {
        _storeAVXRegisterNonTemporal<UnrollFactor>(source, destination);
    }

    static constexpr inline void setZeroAVXRegister(AVXRegister* destination) {
        _setZeroAVXRegister<UnrollFactor>(destination);
    }

    static constexpr inline void setValue(AVXRegister* destination, double value) {
        _setValue<UnrollFactor>(destination, value);
    }

private:
    
    template <unsigned iUnroll>
    static constexpr inline void _loadAVXRegister(const DataType* source, AVXRegister* destination) {
        if constexpr (iUnroll > 0) {
            *(destination + iUnroll - 1) = _mm256_load_pd(source + (iUnroll - 1) * DOUBLE_AVX_REGISTER_SIZE);
            _loadAVXRegister<iUnroll - 1>(source, destination);
        }
    }

    template <unsigned iUnroll>
    static constexpr inline void _storeAVXRegisterTemporal(const AVXRegister* source, DataType* destination) {
        if constexpr (iUnroll > 0) {
            _mm256_store_pd(destination + (iUnroll - 1) * DOUBLE_AVX_REGISTER_SIZE, *(source + iUnroll - 1));
            _storeAVXRegisterTemporal<iUnroll - 1>(source, destination);
        }
    }

    template <unsigned iUnroll>
    static constexpr inline void _storeAVXRegisterNonTemporal(const AVXRegister* source, DataType* destination) {
        if constexpr (iUnroll > 0) {
            _mm256_stream_pd(destination + (iUnroll - 1) * DOUBLE_AVX_REGISTER_SIZE, *(source + iUnroll - 1));
            _storeAVXRegisterNonTemporal<iUnroll - 1>(source, destination);
        }
    }

    template <unsigned iUnroll>
    static constexpr inline void _setZeroAVXRegister(AVXRegister* destination) {
        if constexpr (iUnroll > 0) {
            *(destination + iUnroll - 1) = _mm256_setzero_pd();
            _setZeroAVXRegister<iUnroll - 1>(destination);
        }
    }

    template <unsigned iUnroll>
    static constexpr inline void _setValue(AVXRegister* destination, double value) {
        if constexpr (iUnroll > 0) {
            *(destination + iUnroll - 1) = _mm256_set1_pd(value);
            _setValue<iUnroll - 1>(destination, value);
        }
    }
};

// Int Specialization
template<unsigned int unrollFactor>
struct AVX_Traits<int, unrollFactor> {
    using AVXRegister = __m256i;
    using DataType = int;
    static constexpr unsigned AVXRegisterSize = INT_AVX_REGISTER_SIZE;
    static constexpr unsigned elementsPerCacheLine = 64 / sizeof(int);
    static constexpr unsigned UnrollFactor = unrollFactor;
    AVXRegister avxArray[UnrollFactor];

    constexpr inline __m256i* getAVXRegister() {
        return avxArray;
    }
    
    static constexpr inline void loadAVXRegister(const DataType* source, AVXRegister* destination) {
        _loadAVXRegister<UnrollFactor>(source, destination);
    }

    static constexpr inline void storeAVXRegisterTemporal(const AVXRegister* source, DataType* destination) {
        _storeAVXRegisterTemporal<UnrollFactor>(source, destination);
    }

    static constexpr inline void storeAVXRegisterNonTemporal(const AVXRegister* source, DataType* destination) {
        _storeAVXRegisterNonTemporal<UnrollFactor>(source, destination);
    }

    static constexpr inline void setZeroAVXRegister(AVXRegister* destination) {
        _setZeroAVXRegister<UnrollFactor>(destination);
    }

    static constexpr inline void setValue(AVXRegister* destination, int value) {
        _setValue<UnrollFactor>(destination, value);
    }
    
private:
    template <unsigned iUnroll>
    static constexpr inline void _loadAVXRegister(const DataType* source, AVXRegister* destination) {
        if constexpr (iUnroll > 0) {
            *(destination + iUnroll - 1) = _mm256_load_si256((__m256i *)(source + (iUnroll - 1) * INT_AVX_REGISTER_SIZE));
            _loadAVXRegister<iUnroll - 1>(source, destination);
        }
    }

    template <unsigned iUnroll>
    static constexpr inline void _storeAVXRegisterTemporal(const AVXRegister* source, DataType* destination) {
        if constexpr (iUnroll > 0) {
            _mm256_store_si256((__m256i*)(destination + (iUnroll - 1) * INT_AVX_REGISTER_SIZE), *(source + iUnroll - 1));
            _storeAVXRegisterTemporal<iUnroll - 1>(source, destination);
        }
    }

    template <unsigned iUnroll>
    static constexpr inline void _storeAVXRegisterNonTemporal(const AVXRegister* source, DataType* destination) {
        if constexpr (iUnroll > 0) {
            _mm256_stream_si256((__m256i*)(destination + (iUnroll - 1) * INT_AVX_REGISTER_SIZE), *(source + iUnroll - 1));
            _storeAVXRegisterNonTemporal<iUnroll - 1>(source, destination);
        }
    }

    template <unsigned iUnroll>
    static constexpr inline void _setZeroAVXRegister(AVXRegister* destination) {
        if constexpr (iUnroll > 0) {
            *(destination + iUnroll - 1) = _mm256_setzero_si256();
            _setZeroAVXRegister<iUnroll - 1>(destination);
        }
    }

    template <unsigned iUnroll>
    static constexpr inline void _setValue(AVXRegister* destination, int value) {
        if constexpr (iUnroll > 0) {
            *(destination + iUnroll - 1) = _mm256_set1_epi32(value);
            _setValue<iUnroll - 1>(destination, value);
        }
    }
};

template<unsigned int unrollFactor>
struct AVX_Traits<short, unrollFactor> {
    using AVXRegister = __m256i;
    using DataType = short;
    static constexpr unsigned AVXRegisterSize = SHORT_AVX_REGISTER_SIZE;
    static constexpr unsigned elementsPerCacheLine = 64 / sizeof(short);
    static constexpr unsigned UnrollFactor = unrollFactor;
    AVXRegister avxArray[UnrollFactor];

    constexpr inline AVXRegister* getAVXRegister() {
        return avxArray;
    }

    static constexpr inline void loadAVXRegister(const DataType* source, AVXRegister* destination) {
        _loadAVXRegister<UnrollFactor>(source, destination);
    }

    static constexpr inline void storeAVXRegisterTemporal(const AVXRegister* source, DataType* destination) {
        _storeAVXRegisterTemporal<UnrollFactor>(source, destination);
    }

    static constexpr inline void storeAVXRegisterNonTemporal(const AVXRegister* source, DataType* destination) {
        _storeAVXRegisterNonTemporal<UnrollFactor>(source, destination);
    }

    static constexpr inline void setZeroAVXRegister(AVXRegister* destination) {
        _setZeroAVXRegister<UnrollFactor>(destination);
    }

    static constexpr inline void setValue(AVXRegister* destination, short value) {
        _setValue<UnrollFactor>(destination, value);
    }
    
private:
    template <unsigned iUnroll>
    static constexpr inline void _loadAVXRegister(const DataType* source, AVXRegister* destination) {
        if constexpr (iUnroll > 0) {
            *(destination + iUnroll - 1) = _mm256_load_si256((__m256i *)(source + (iUnroll - 1) * SHORT_AVX_REGISTER_SIZE));
            _loadAVXRegister<iUnroll - 1>(source, destination);
        }
    }

    template <unsigned iUnroll>
    static constexpr inline void _storeAVXRegisterTemporal(const AVXRegister* source, DataType* destination) {
        if constexpr (iUnroll > 0) {
            _mm256_store_si256((__m256i*)(destination + (iUnroll - 1) * SHORT_AVX_REGISTER_SIZE), *(source + iUnroll - 1));
            _storeAVXRegisterTemporal<iUnroll - 1>(source, destination);
        }
    }

    template <unsigned iUnroll>
    static constexpr inline void _storeAVXRegisterNonTemporal(const AVXRegister* source, DataType* destination) {
        if constexpr (iUnroll > 0) {
            _mm256_stream_si256((__m256i*)(destination + (iUnroll - 1) * SHORT_AVX_REGISTER_SIZE), *(source + iUnroll - 1));
            _storeAVXRegisterNonTemporal<iUnroll - 1>(source, destination);
        }
    }

    template <unsigned iUnroll>
    static constexpr inline void _setZeroAVXRegister(AVXRegister* destination) {
        if constexpr (iUnroll > 0) {
            *(destination + iUnroll - 1) = _mm256_setzero_si256();
            _setZeroAVXRegister<iUnroll - 1>(destination);
        }
    }

    template <unsigned iUnroll>
    static constexpr inline void _setValue(AVXRegister* destination, short value) {
        if constexpr (iUnroll > 0) {
            *(destination + iUnroll - 1) = _mm256_set1_epi16(value);
            _setValue<iUnroll - 1>(destination, value);
        }
    }
};

template<unsigned int unrollFactor>
struct AVX_Traits<unsigned, unrollFactor> {
    using AVXRegister = __m256i;
    using DataType = unsigned;
    static constexpr unsigned AVXRegisterSize = UNSIGNED_AVX_REGISTER_SIZE;
    static constexpr unsigned elementsPerCacheLine = 64 / sizeof(unsigned);
    static constexpr unsigned UnrollFactor = unrollFactor;
    AVXRegister avxArray[UnrollFactor];

    constexpr inline AVXRegister* getAVXRegister() {
        return avxArray;
    }

    static constexpr inline void loadAVXRegister(const DataType* source, AVXRegister* destination) {
        _loadAVXRegister<UnrollFactor>(source, destination);
    }

    static constexpr inline void storeAVXRegisterTemporal(const AVXRegister* source, DataType* destination) {
        _storeAVXRegisterTemporal<UnrollFactor>(source, destination);
    }

    static constexpr inline void storeAVXRegisterNonTemporal(const AVXRegister* source, DataType* destination) {
        _storeAVXRegisterNonTemporal<UnrollFactor>(source, destination);
    }

    static constexpr inline void setZeroAVXRegister(AVXRegister* destination) {
        _setZeroAVXRegister<UnrollFactor>(destination);
    }

    static constexpr inline void setValue(AVXRegister* destination, unsigned value) {
        _setValue<UnrollFactor>(destination, value);
    }
    
private:
    template <unsigned iUnroll>
    static constexpr inline void _loadAVXRegister(const DataType* source, AVXRegister* destination) {
        if constexpr (iUnroll > 0) {
            *(destination + iUnroll - 1) = _mm256_load_si256((__m256i *)(source + (iUnroll - 1) * UNSIGNED_AVX_REGISTER_SIZE));
            _loadAVXRegister<iUnroll - 1>(source, destination);
        }
    }

    template <unsigned iUnroll>
    static constexpr inline void _storeAVXRegisterTemporal(const AVXRegister* source, DataType* destination) {
        if constexpr (iUnroll > 0) {
            _mm256_store_si256((__m256i*)(destination + (iUnroll - 1) * UNSIGNED_AVX_REGISTER_SIZE), *(source + iUnroll - 1));
            _storeAVXRegisterTemporal<iUnroll - 1>(source, destination);
        }
    }

    template <unsigned iUnroll>
    static constexpr inline void _storeAVXRegisterNonTemporal(const AVXRegister* source, DataType* destination) {
        if constexpr (iUnroll > 0) {
            _mm256_stream_si256((__m256i*)(destination + (iUnroll - 1) * UNSIGNED_AVX_REGISTER_SIZE), *(source + iUnroll - 1));
            _storeAVXRegisterNonTemporal<iUnroll - 1>(source, destination);
        }
    }

    template <unsigned iUnroll>
    static constexpr inline void _setZeroAVXRegister(AVXRegister* destination) {
        if constexpr (iUnroll > 0) {
            *(destination + iUnroll - 1) = _mm256_setzero_si256();
            _setZeroAVXRegister<iUnroll - 1>(destination);
        }
    }

    template <unsigned iUnroll>
    static constexpr inline void _setValue(AVXRegister* destination, unsigned value) {
        if constexpr (iUnroll > 0) {
            *(destination + iUnroll - 1) = _mm256_set1_epi32(value);
            _setValue<iUnroll - 1>(destination, value);
        }
    }
};


    

class AVX_MemoryManagement {

//    using PrefetchFunction = std::function<void(T*)>;
//
//    using StoreDoubleFunction = std::function<void(__m256d*, T*)>;
    
    public:

    template<typename T, unsigned unrollFactor>
    static AVX_Traits<T, unrollFactor> getAVXTraits() {
        return AVX_Traits<T,unrollFactor>();
    }

    template <typename T>
    static constexpr inline T* allocate(unsigned size, unsigned alignment) {
        void *allocatedData = aligned_alloc(alignment, size * sizeof(T));
        if (allocatedData == nullptr)
            throw std::runtime_error("Memory allocation failed.");
        return static_cast<T*>(allocatedData);
    }

    template <typename T>
    static constexpr inline void deallocate(T *ptr) {
        free(ptr);
    }
    
    template <typename T>
    static constexpr inline bool checkInputType() {
        return std::is_same<T, float>::value || 
               std::is_same<T, double>::value ||
               std::is_same<T, int>::value ||
               std::is_same<T, short>::value ||
               std::is_same<T, unsigned>::value;
    }


    template <typename T>
    static constexpr inline void prefetch(const T *data, _mm_hint hint) {
        switch (hint) {
            case _MM_HINT_T0:
                _prefetchCacheLine_T0(data);
                break;
            case _MM_HINT_T1:
                _prefetchCacheLine_T1(data);
                break;
            case _MM_HINT_T2:
                _prefetchCacheLine_T2(data);
                break;
            case _MM_HINT_NTA:
                _prefetchCacheLine_NTA(data);
                break;
            default:
                throw std::invalid_argument("Invalid hint");
        }
    }
    

    template <typename T, typename avxData, unsigned iUnroll = 1>
    static constexpr inline void loadDataRegister(const T *source, avxData *avxDestination) {
        if constexpr (iUnroll > 0) {
            if constexpr (_isDouble<T>())
                *(avxDestination + iUnroll - 1) = _mm256_load_pd(source + (iUnroll - 1) * DOUBLE_AVX_REGISTER_SIZE);
            else if constexpr (_isFloat<T>())
                *(avxDestination + iUnroll - 1) = _mm256_load_ps(source + (iUnroll - 1) * FLOAT_AVX_REGISTER_SIZE);
            else if constexpr
                (_isInt<T>() || _isUnsigned<T>()) *(avxDestination + iUnroll - 1) = _mm256_load_si256((__m256i *)(source + (iUnroll - 1) * INT_AVX_REGISTER_SIZE));
            else if constexpr
                (_isShort<T>()) *(avxDestination + iUnroll - 1) = _mm256_load_si256((__m256i *)(source + (iUnroll - 1) * SHORT_AVX_REGISTER_SIZE));
            loadDataRegister<T, avxData, iUnroll - 1>(source, avxDestination);
        }
        else return;
    }

    template <typename T, typename avxData, unsigned iUnroll, unsigned iVector>
    static constexpr inline void
    loadMultipleDataRegisters(double **source, avxData *avxDestination, unsigned unrollFactor, unsigned index) {
        if constexpr (iVector > 0) {
            loadDataRegister<T, __m256d, iUnroll>(*(source + iVector - 1) + index, avxDestination + (iVector - 1) * unrollFactor);
            loadMultipleDataRegisters<T, avxData, iUnroll, iVector - 1>(source, avxDestination, unrollFactor, index);
        }
        else return;
    }
    
    template<typename T, typename avxData, unsigned iScalar = 1>
    static constexpr inline void broadcastScalars(const double *source, avxData *avxDestination) {
        if constexpr (iScalar > 0) {
            if constexpr (_isDouble<T>())
                *(avxDestination + iScalar - 1) = _mm256_set1_pd(*(source + iScalar - 1));
            else if constexpr (_isFloat<T>())
                *(avxDestination + iScalar - 1) = _mm256_set1_ps(*(source + iScalar - 1));
            else if constexpr (_isInt<T>() || _isUnsigned<T>())
                *(avxDestination + iScalar - 1) = _mm256_set1_epi32(*(source + iScalar - 1));
            else if constexpr (_isShort<T>())
                *(avxDestination + iScalar - 1) = _mm256_set1_epi16(*(source + iScalar - 1));
            broadcastScalars<T, avxData, iScalar - 1>(source, avxDestination);
        }
        else return;
    }
    
    template<typename T, typename avxData, unsigned iUnroll = 1>
    static constexpr inline void setZero(avxData *avxDestination) {
        if constexpr (iUnroll > 0) {
            if constexpr (_isDouble<T>())
                *(avxDestination + iUnroll - 1) = _mm256_setzero_pd();
            else if constexpr (_isFloat<T>())
                *(avxDestination + iUnroll - 1) = _mm256_setzero_ps();
            else if constexpr (_isInt<T>() || _isUnsigned<T>() || _isShort<T>())
                *(avxDestination + iUnroll - 1) = _mm256_setzero_si256();
            setZero<T, avxData, iUnroll - 1>(avxDestination);
        }
        else return;
    }

    template<typename T, typename avxData, unsigned iUnroll = 1>
    static constexpr inline void storeTemporalData(avxData* source, T* destination) {
        if constexpr (iUnroll > 0) {
            if constexpr (_isDouble<T>()) {
                _mm256_store_pd((double*)(destination + (iUnroll - 1) * 4), *(source + iUnroll - 1));
            } else if constexpr (_isFloat<T>()) {
                _mm256_store_ps((float*)(destination + (iUnroll - 1) * 8), *(source + iUnroll - 1));
            } else if constexpr (_isInt<T>() || _isUnsigned<T>() || _isShort<T>()) {
                _mm256_store_si256((__m256i*)(destination + (iUnroll - 1) * 8), *(source + iUnroll - 1));
            }
            storeTemporalData<T, avxData, iUnroll - 1>(source, destination);
        }
    }

    template<typename T, typename avxData, unsigned iUnroll = 1>
    static constexpr inline void storeNonTemporalData(avxData* source, T* destination) {
        if constexpr (iUnroll > 0) {
            if constexpr (_isDouble<T>()) {
                _mm256_stream_pd(destination + (iUnroll - 1) * 4, *(source + iUnroll - 1));
            } else if constexpr (_isFloat<T>()) {
                _mm256_stream_ps(destination + (iUnroll - 1) * 8, *(source + iUnroll - 1));
            } else if constexpr (_isInt<T>() || _isUnsigned<T>() || _isShort<T>()) {
                _mm256_stream_si256((__m256i*)(destination + (iUnroll - 1) * 8), *(source + iUnroll - 1));
            }
            storeNonTemporalData<T, avxData, iUnroll - 1>(source, destination);
        }
    }
    
    private:

    template <typename T>
    static constexpr inline void _prefetchCacheLine_T0(T *data) {
        _mm_prefetch(reinterpret_cast<const char*>(data), _MM_HINT_T0);
    }

    template <typename T>
    static constexpr inline void _prefetchCacheLine_T1(T *data) {
        _mm_prefetch(reinterpret_cast<const char*>(data), _MM_HINT_T1);
    }

    template <typename T>
    static constexpr inline void _prefetchCacheLine_T2(T *data) {
        _mm_prefetch(reinterpret_cast<const char*>(data), _MM_HINT_T2);
    }

    template <typename T>
    static constexpr inline void _prefetchCacheLine_NTA(T *data) {
        _mm_prefetch(reinterpret_cast<const char*>(data), _MM_HINT_NTA);
    }
    
    template<typename T>
    static constexpr inline bool _isFloat() {
        return std::is_same<T, float>::value;
    }
    
    template<typename T>
    static constexpr inline bool _isDouble() {
        return std::is_same<T, double>::value;
    }
    
    template<typename T>
    static constexpr inline bool _isInt() {
        return std::is_same<T, int>::value;
    }
    
    template<typename T>
    static constexpr inline bool _isUnsigned() {
        return std::is_same<T, unsigned>::value;
    }
        
    template<typename T>
    static constexpr inline bool _isShort() {
        return std::is_same<T, short>::value;
    }
    
};


#endif //STALKER_AVX_MEMORYMANAGEMENT_H
