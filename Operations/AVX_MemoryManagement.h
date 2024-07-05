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


class AVX_MemoryManagement {

//    using PrefetchFunction = std::function<void(T*)>;
//
//    using StoreDoubleFunction = std::function<void(__m256d*, T*)>;
    
    public:

    const std::unordered_map<std::type_index, unsigned> STLKR_SIMD_DataTypeRegisterSize = {
            {std::type_index(typeid(float)),    FLOAT_AVX_REGISTER_SIZE},
            {std::type_index(typeid(double)),   DOUBLE_AVX_REGISTER_SIZE},
            {std::type_index(typeid(int)),      INT_AVX_REGISTER_SIZE},
            {std::type_index(typeid(short)),    SHORT_AVX_REGISTER_SIZE},
            {std::type_index(typeid(unsigned)), UNSIGNED_AVX_REGISTER_SIZE}
    };

    template <typename T>
    static constexpr inline void allocate(unsigned size, unsigned alignment) {
        aligned_alloc(alignment, size * sizeof(T));
    }

    template <typename T>
    static constexpr inline void deallocate(T *ptr) {
        free(ptr);
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

//constexpr void _storeDoubleTemporal(__m256d *src, double *dest) {
//    _mm256_stream_pd(dest, *src);
//}
//
//constexpr void _storeDoubleNonTemporal(__m256d *src, double *dest) {
//    _mm256_store_pd(dest, *src);
//}

    
    
    
    
};


#endif //STALKER_AVX_MEMORYMANAGEMENT_H
