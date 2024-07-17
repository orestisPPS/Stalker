//
// Created by hal9000 on 6/7/24.
//

#ifndef STALKER_AVX_MEMORYMANAGEMENT_H
#define STALKER_AVX_MEMORYMANAGEMENT_H

#include <cstdlib>
#include <immintrin.h>
#include <stdexcept>
#include <unordered_map>
#include <functional>
#include <typeindex>
#include "AVX_MemoryTraits.h"

class AVX_MemoryManagement {

//    using PrefetchFunction = std::function<void(T*)>;
//
//    using StoreDoubleFunction = std::function<void(__m256d*, T*)>;
    
    public:

    template <typename T>
    static T* allocate(unsigned size, unsigned alignment = 64) {
        void* allocatedData = _mm_malloc(size * sizeof(T), alignment);
        if (allocatedData == nullptr) {
            throw std::runtime_error("Memory allocation failed.");
        }
        if (reinterpret_cast<std::uintptr_t>(allocatedData) % alignment != 0) {
            throw std::runtime_error("Memory allocation did not meet alignment requirements.");
        }
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
