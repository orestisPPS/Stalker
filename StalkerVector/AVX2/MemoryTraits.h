//
// Created by hal9000 on 7/8/24.
//

#ifndef STALKER_MemoryTraits_H
#define STALKER_MemoryTraits_H
// AVX register sizes in terms of elements per register
#define DOUBLE_AVX_REGISTER_SIZE 4     // 256 bits / 64 bits per double
#define FLOAT_AVX_REGISTER_SIZE 8      // 256 bits / 32 bits per float
#define INT_AVX_REGISTER_SIZE 8        // 256 bits / 32 bits per int
#define SHORT_AVX_REGISTER_SIZE 16     // 256 bits / 16 bits per short
#define UNSIGNED_AVX_REGISTER_SIZE 8   // 256 bits / 32 bits per unsigned int

#include <immintrin.h>

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

template<typename T, unsigned int unrollFactor>
struct MemoryTraits;

// Float Specialization
template<unsigned int unrollFactor>
struct MemoryTraits<float, unrollFactor> {
    using T_simd = __m256;
    using T_data = float;
    static constexpr unsigned registerSize = FLOAT_AVX_REGISTER_SIZE;
    static constexpr unsigned elementsPerCacheLine = 64 / sizeof(float);
    static constexpr unsigned UnrollFactor = unrollFactor;
    static constexpr unsigned cacheLinesProcessed = (unrollFactor * registerSize) / elementsPerCacheLine;
    static constexpr unsigned blockSize = registerSize * UnrollFactor;
    
    static constexpr inline void loadRegister(const T_data* source, T_simd* destination) {
        _loadAVXRegister<UnrollFactor>(source, destination);
    }

    static constexpr inline void temporalStore(const T_simd* source, T_data* destination) {
        _storeAVXRegisterTemporal<UnrollFactor>(source, destination);
    }

    static constexpr inline void nonTemporalStore(const T_simd* source, T_data* destination) {
        _storeAVXRegisterNonTemporal<UnrollFactor>(source, destination);
    }

    static constexpr inline void setZeroAVXRegister(T_simd* destination) {
        _setZeroAVXRegister<UnrollFactor>(destination);
    }

    static constexpr inline void setValue(T_simd* destination, const T_data& value) {
        _setValue<UnrollFactor>(destination, value);
    }
    
    static constexpr inline bool areEqual(const T_simd* a, const T_simd* b) {
        return _areEqual<UnrollFactor>(a, b);
    }

private:
    template <unsigned iUnroll>
    static constexpr inline void _loadAVXRegister(const T_data* source, T_simd* destination) {
        if constexpr (iUnroll > 0) {
            *(destination + iUnroll - 1) = _mm256_load_ps(source + (iUnroll - 1) * FLOAT_AVX_REGISTER_SIZE);
            _loadAVXRegister<iUnroll - 1>(source, destination);
        }
        else return;
    }

    template <unsigned iUnroll>
    static constexpr inline void _storeAVXRegisterTemporal(const T_simd* source, T_data* destination) {
        if constexpr (iUnroll > 0) {
            _mm256_store_ps(destination + (iUnroll - 1) * FLOAT_AVX_REGISTER_SIZE, *(source + iUnroll - 1));
            _storeAVXRegisterTemporal<iUnroll - 1>(source, destination);
        }
        else return;
    }

    template <unsigned iUnroll>
    static constexpr inline void _storeAVXRegisterNonTemporal(const T_simd* source, T_data* destination) {
        if constexpr (iUnroll > 0) {
            _mm256_stream_ps(destination + (iUnroll - 1) * FLOAT_AVX_REGISTER_SIZE, *(source + iUnroll - 1));
            _storeAVXRegisterNonTemporal<iUnroll - 1>(source, destination);
        }
        else return;
    }

    template <unsigned iUnroll>
    static constexpr inline void _setZeroAVXRegister(T_simd* destination) {
        if constexpr (iUnroll > 0) {
            *(destination + iUnroll - 1) = _mm256_setzero_ps();
            _setZeroAVXRegister<iUnroll - 1>(destination);
        }
        else return;
    }
    
    template <unsigned iUnroll>
    static constexpr inline void _setValue(T_simd* destination, const T_data& value) {
        if constexpr (iUnroll > 0) {
            *(destination + iUnroll - 1) = _mm256_set1_ps(value);
            _setValue<iUnroll - 1>(destination, value);
        }
        else return;
    }
    
    template <unsigned iUnroll>
    static constexpr inline bool _areEqual(const T_simd* a, const T_simd* b) {
        if constexpr (iUnroll > 0) {
            if (_mm256_movemask_ps(_mm256_cmp_ps(*(a + iUnroll - 1), *(b + iUnroll - 1), _CMP_NEQ_UQ)) != 0) {
                return false;
            }
            _areEqual<iUnroll - 1>(a, b);
        }
        else return true;
    }
};

// Double Specialization
template<unsigned int unrollFactor>
struct MemoryTraits<double, unrollFactor> {
    using T_simd = __m256d;
    using T_data = double;
    static constexpr unsigned registerSize = DOUBLE_AVX_REGISTER_SIZE;
    static constexpr unsigned elementsPerCacheLine = 64 / sizeof(double);
    static constexpr unsigned UnrollFactor = unrollFactor;
    static constexpr unsigned cacheLinesProcessed = (unrollFactor * registerSize) / elementsPerCacheLine;
    static constexpr unsigned blockSize = registerSize * UnrollFactor;

    static constexpr inline void loadRegister(const T_data* source, T_simd* destination) {
        _loadAVXRegister<UnrollFactor>(source, destination);
    }

    static constexpr inline void temporalStore(const T_simd* source, T_data* destination) {
        _storeAVXRegisterTemporal<UnrollFactor>(source, destination);
    }

    static constexpr inline void nonTemporalStore(const T_simd* source, T_data* destination) {
        _storeAVXRegisterNonTemporal<UnrollFactor>(source, destination);
    }

    static constexpr inline void setZeroAVXRegister(T_simd* destination) {
        _setZeroAVXRegister<UnrollFactor>(destination);
    }

    static constexpr inline void setValue(T_simd* destination, double value) {
        _setValue<UnrollFactor>(destination, value);
    }
    
    static constexpr inline bool areEqual(const T_simd* a, const T_simd* b) {
        return _areEqual<UnrollFactor>(a, b);
    }

private:

    template <unsigned iUnroll>
    static constexpr inline void _loadAVXRegister(const T_data* source, T_simd* destination) {
        if constexpr (iUnroll > 0) {
            *(destination + iUnroll - 1) = _mm256_load_pd(source + (iUnroll - 1) * DOUBLE_AVX_REGISTER_SIZE);
            _loadAVXRegister<iUnroll - 1>(source, destination);
        }
    }

    template <unsigned iUnroll>
    static constexpr inline void _storeAVXRegisterTemporal(const T_simd* source, T_data* destination) {
        if constexpr (iUnroll > 0) {
            _mm256_store_pd(destination + (iUnroll - 1) * DOUBLE_AVX_REGISTER_SIZE, *(source + iUnroll - 1));
            _storeAVXRegisterTemporal<iUnroll - 1>(source, destination);
        }
    }

    template <unsigned iUnroll>
    static constexpr inline void _storeAVXRegisterNonTemporal(const T_simd* source, T_data* destination) {
        if constexpr (iUnroll > 0) {
            _mm256_stream_pd(destination + (iUnroll - 1) * DOUBLE_AVX_REGISTER_SIZE, *(source + iUnroll - 1));
            _storeAVXRegisterNonTemporal<iUnroll - 1>(source, destination);
        }
    }

    template <unsigned iUnroll>
    static constexpr inline void _setZeroAVXRegister(T_simd* destination) {
        if constexpr (iUnroll > 0) {
            *(destination + iUnroll - 1) = _mm256_setzero_pd();
            _setZeroAVXRegister<iUnroll - 1>(destination);
        }
    }

    template <unsigned iUnroll>
    static constexpr inline void _setValue(T_simd* destination, const T_data& value) {
        if constexpr (iUnroll > 0) {
            *(destination + iUnroll - 1) = _mm256_set1_pd(value);
            _setValue<iUnroll - 1>(destination, value);
        }
    }
    
    template <unsigned iUnroll>
    static constexpr inline bool _areEqual(const T_simd* a, const T_simd* b) {
        if constexpr (iUnroll > 0) {
            if (_mm256_movemask_pd(_mm256_cmp_pd(*(a + iUnroll - 1), *(b + iUnroll - 1), _CMP_NEQ_UQ)) != 0) {
                return false;
            }
            _areEqual<iUnroll - 1>(a, b);
        }
        else return true;
    }
};

// Int Specialization
template<unsigned int unrollFactor>
struct MemoryTraits<int, unrollFactor> {
    using T_simd = __m256i;
    using T_data = int;
    static constexpr unsigned registerSize = INT_AVX_REGISTER_SIZE;
    static constexpr unsigned elementsPerCacheLine = 64 / sizeof(int);
    static constexpr unsigned UnrollFactor = unrollFactor;
    static constexpr unsigned cacheLinesProcessed = (unrollFactor * registerSize) / elementsPerCacheLine;
    static constexpr unsigned blockSize = registerSize * UnrollFactor;
    
    static constexpr inline void loadRegister(const T_data* source, T_simd* destination) {
        _loadAVXRegister<UnrollFactor>(source, destination);
    }

    static constexpr inline void temporalStore(const T_simd* source, T_data* destination) {
        _storeAVXRegisterTemporal<UnrollFactor>(source, destination);
    }

    static constexpr inline void nonTemporalStore(const T_simd* source, T_data* destination) {
        _storeAVXRegisterNonTemporal<UnrollFactor>(source, destination);
    }

    static constexpr inline void setZeroAVXRegister(T_simd* destination) {
        _setZeroAVXRegister<UnrollFactor>(destination);
    }

    static constexpr inline void setValue(T_simd* destination, int value) {
        _setValue<UnrollFactor>(destination, value);
    }

    static constexpr inline bool areEqual(const T_simd* a, const T_simd* b) {
        return _areEqual<UnrollFactor>(a, b);
    }

private:
    template <unsigned iUnroll>
    static constexpr inline void _loadAVXRegister(const T_data* source, T_simd* destination) {
        if constexpr (iUnroll > 0) {
            *(destination + iUnroll - 1) = _mm256_load_si256(reinterpret_cast<const __m256i*>(source + (iUnroll - 1) * INT_AVX_REGISTER_SIZE));
            _loadAVXRegister<iUnroll - 1>(source, destination);
        }
    }

    template <unsigned iUnroll>
    static constexpr inline void _storeAVXRegisterTemporal(const T_simd* source, T_data* destination) {
        if constexpr (iUnroll > 0) {
            _mm256_store_si256(reinterpret_cast<__m256i*>(destination + (iUnroll - 1) * INT_AVX_REGISTER_SIZE), *(source + iUnroll - 1));
            _storeAVXRegisterTemporal<iUnroll - 1>(source, destination);
        }
    }

    template <unsigned iUnroll>
    static constexpr inline void _storeAVXRegisterNonTemporal(const T_simd* source, T_data* destination) {
        if constexpr (iUnroll > 0) {
            _mm256_stream_si256(reinterpret_cast<__m256i*>(destination + (iUnroll - 1) * INT_AVX_REGISTER_SIZE), *(source + iUnroll - 1));
            _storeAVXRegisterNonTemporal<iUnroll - 1>(source, destination);
        }
    }

    template <unsigned iUnroll>
    static constexpr inline void _setZeroAVXRegister(T_simd* destination) {
        if constexpr (iUnroll > 0) {
            *(destination + iUnroll - 1) = _mm256_setzero_si256();
            _setZeroAVXRegister<iUnroll - 1>(destination);
        }
    }

    template <unsigned iUnroll>
    static constexpr inline void _setValue(T_simd* destination, const T_data& value) {
        if constexpr (iUnroll > 0) {
            *(destination + iUnroll - 1) = _mm256_set1_epi32(value);
            _setValue<iUnroll - 1>(destination, value);
        }
    }

    template <unsigned iUnroll>
    static constexpr inline bool _areEqual(const T_simd* a, const T_simd* b) {
        if constexpr (iUnroll > 0) {
            if (_mm256_movemask_epi8(_mm256_cmpeq_epi32(*(a + iUnroll - 1), *(b + iUnroll - 1))) != 0xFFFFFFFF) {
                return false;
            }
            _areEqual<iUnroll - 1>(a, b);
        }
        else return true;
    }
};

template<unsigned int unrollFactor>
struct MemoryTraits<short, unrollFactor> {
    using T_simd = __m256i;
    using T_data = short;
    static constexpr unsigned registerSize = SHORT_AVX_REGISTER_SIZE;
    static constexpr unsigned elementsPerCacheLine = 64 / sizeof(short);
    static constexpr unsigned UnrollFactor = unrollFactor;
    static constexpr unsigned cacheLinesProcessed = (unrollFactor * registerSize) / elementsPerCacheLine;
    static constexpr unsigned blockSize = registerSize * UnrollFactor;

    static constexpr inline void loadRegister(const T_data* source, T_simd* destination) {
        _loadAVXRegister<UnrollFactor>(source, destination);
    }

    static constexpr inline void temporalStore(const T_simd* source, T_data* destination) {
        _storeAVXRegisterTemporal<UnrollFactor>(source, destination);
    }

    static constexpr inline void nonTemporalStore(const T_simd* source, T_data* destination) {
        _storeAVXRegisterNonTemporal<UnrollFactor>(source, destination);
    }

    static constexpr inline void setZeroAVXRegister(T_simd* destination) {
        _setZeroAVXRegister<UnrollFactor>(destination);
    }

    static constexpr inline void setValue(T_simd* destination, short value) {
        _setValue<UnrollFactor>(destination, value);
    }

    static constexpr inline bool areEqual(const T_simd* a, const T_simd* b) {
        return _areEqual<UnrollFactor>(a, b);
    }

private:
    template <unsigned iUnroll>
    static constexpr inline void _loadAVXRegister(const T_data* source, T_simd* destination) {
        if constexpr (iUnroll > 0) {
            *(destination + iUnroll - 1) = _mm256_load_si256(reinterpret_cast<const __m256i*>(source + (iUnroll - 1) * SHORT_AVX_REGISTER_SIZE));
            _loadAVXRegister<iUnroll - 1>(source, destination);
        }
    }

    template <unsigned iUnroll>
    static constexpr inline void _storeAVXRegisterTemporal(const T_simd* source, T_data* destination) {
        if constexpr (iUnroll > 0) {
            _mm256_store_si256(reinterpret_cast<__m256i*>(destination + (iUnroll - 1) * SHORT_AVX_REGISTER_SIZE), *(source + iUnroll - 1));
            _storeAVXRegisterTemporal<iUnroll - 1>(source, destination);
        }
    }

    template <unsigned iUnroll>
    static constexpr inline void _storeAVXRegisterNonTemporal(const T_simd* source, T_data* destination) {
        if constexpr (iUnroll > 0) {
            _mm256_stream_si256(reinterpret_cast<__m256i*>(destination + (iUnroll - 1) * SHORT_AVX_REGISTER_SIZE), *(source + iUnroll - 1));
            _storeAVXRegisterNonTemporal<iUnroll - 1>(source, destination);
        }
    }

    template <unsigned iUnroll>
    static constexpr inline void _setZeroAVXRegister(T_simd* destination) {
        if constexpr (iUnroll > 0) {
            *(destination + iUnroll - 1) = _mm256_setzero_si256();
            _setZeroAVXRegister<iUnroll - 1>(destination);
        }
    }

    template <unsigned iUnroll>
    static constexpr inline void _setValue(T_simd* destination, const T_data& value) {
        if constexpr (iUnroll > 0) {
            *(destination + iUnroll - 1) = _mm256_set1_epi16(value);
            _setValue<iUnroll - 1>(destination, value);
        }
    }

    template <unsigned iUnroll>
    static constexpr inline bool _areEqual(const T_simd* a, const T_simd* b) {
        if constexpr (iUnroll > 0) {
            if (_mm256_movemask_epi8(_mm256_cmpeq_epi16(*(a + iUnroll - 1), *(b + iUnroll - 1))) != 0xFFFFFFFF) {
                return false;
            }
            _areEqual<iUnroll - 1>(a, b);
        }
        else return true;
    }
};

template<unsigned int unrollFactor>
struct MemoryTraits<unsigned, unrollFactor> {
    using T_simd = __m256i;
    using T_data = unsigned;
    static constexpr unsigned registerSize = UNSIGNED_AVX_REGISTER_SIZE;
    static constexpr unsigned elementsPerCacheLine = 64 / sizeof(unsigned);
    static constexpr unsigned UnrollFactor = unrollFactor;
    static constexpr unsigned cacheLinesProcessed = (unrollFactor * registerSize) / elementsPerCacheLine;
    static constexpr unsigned blockSize = registerSize * UnrollFactor;

    static constexpr inline void loadRegister(const T_data* source, T_simd* destination) {
        _loadAVXRegister<UnrollFactor>(source, destination);
    }

    static constexpr inline void temporalStore(const T_simd* source, T_data* destination) {
        _storeAVXRegisterTemporal<UnrollFactor>(source, destination);
    }

    static constexpr inline void nonTemporalStore(const T_simd* source, T_data* destination) {
        _storeAVXRegisterNonTemporal<UnrollFactor>(source, destination);
    }

    static constexpr inline void setZeroAVXRegister(T_simd* destination) {
        _setZeroAVXRegister<UnrollFactor>(destination);
    }

    static constexpr inline void setValue(T_simd* destination, unsigned value) {
        _setValue<UnrollFactor>(destination, value);
    }

    static constexpr inline bool areEqual(const T_simd* a, const T_simd* b) {
        return _areEqual<UnrollFactor>(a, b);
    }

private:
    template <unsigned iUnroll>
    static constexpr inline void _loadAVXRegister(const T_data* source, T_simd* destination) {
        if constexpr (iUnroll > 0) {
            *(destination + iUnroll - 1) = _mm256_load_si256(reinterpret_cast<const __m256i*>(source + (iUnroll - 1) * UNSIGNED_AVX_REGISTER_SIZE));
            _loadAVXRegister<iUnroll - 1>(source, destination);
        }
    }

    template <unsigned iUnroll>
    static constexpr inline void _storeAVXRegisterTemporal(const T_simd* source, T_data* destination) {
        if constexpr (iUnroll > 0) {
            _mm256_store_si256(reinterpret_cast<__m256i*>(destination + (iUnroll - 1) * UNSIGNED_AVX_REGISTER_SIZE), *(source + iUnroll - 1));
            _storeAVXRegisterTemporal<iUnroll - 1>(source, destination);
        }
    }

    template <unsigned iUnroll>
    static constexpr inline void _storeAVXRegisterNonTemporal(const T_simd* source, T_data* destination) {
        if constexpr (iUnroll > 0) {
            _mm256_stream_si256(reinterpret_cast<__m256i*>(destination + (iUnroll - 1) * UNSIGNED_AVX_REGISTER_SIZE), *(source + iUnroll - 1));
            _storeAVXRegisterNonTemporal<iUnroll - 1>(source, destination);
        }
    }

    template <unsigned iUnroll>
    static constexpr inline void _setZeroAVXRegister(T_simd* destination) {
        if constexpr (iUnroll > 0) {
            *(destination + iUnroll - 1) = _mm256_setzero_si256();
            _setZeroAVXRegister<iUnroll - 1>(destination);
        }
    }

    template <unsigned iUnroll>
    static constexpr inline void _setValue(T_simd* destination, const T_data& value) {
        if constexpr (iUnroll > 0) {
            *(destination + iUnroll - 1) = _mm256_set1_epi32(value);
            _setValue<iUnroll - 1>(destination, value);
        }
    }

    template <unsigned iUnroll>
    static constexpr inline bool _areEqual(const T_simd* a, const T_simd* b) {
        if constexpr (iUnroll > 0) {
            if (_mm256_movemask_epi8(_mm256_cmpeq_epi32(*(a + iUnroll - 1), *(b + iUnroll - 1))) != 0xFFFFFFFF) {
                return false;
            }
            _areEqual<iUnroll - 1>(a, b);
        }
        else return true;
    }
};
#endif //STALKER_MemoryTraits_H
