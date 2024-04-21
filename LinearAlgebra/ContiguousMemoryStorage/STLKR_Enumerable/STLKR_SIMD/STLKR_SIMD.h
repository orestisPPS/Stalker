//
// Created by hal9000 on 4/21/24.
//

#ifndef STALKER_STLKR_SIMD_H
#define STALKER_STLKR_SIMD_H

#include <map>
#include <typeindex>
#include <immintrin.h>
#include <cstddef>
#include <stdexcept>

#define AVX_DOUBLE_SIZE 4
#define AVX_FLOAT_SIZE 8
#define AVX_INT_SIZE 8
#define AVX_SHORT_SIZE 16
#define AVX_UNSIGNED_SIZE 8

using namespace std;

namespace STLKR_LinearAlgebra {
    
    static const std::map<std::type_index, unsigned> STLKR_SIMD_DataTypeSize = {
            {std::type_index(typeid(float)), AVX_FLOAT_SIZE},
            {std::type_index(typeid(double)), AVX_DOUBLE_SIZE},
            {std::type_index(typeid(int)), AVX_INT_SIZE},
            {std::type_index(typeid(short)), AVX_SHORT_SIZE},
            {std::type_index(typeid(unsigned)), AVX_UNSIGNED_SIZE}
    };
    
    
    /**
     * @brief Enum class representing the various types of AVX support.
     */
    enum class SIMD_AVXType {
        AVX,     ///< Represents support for AVX with 256-bit wide registers. Useful for operations that benefit from wider data throughput compared to earlier SSE instructions.
        AVX_2,     ///< Represents support for AVX2, an extension to the original AVX, including enhancements such as wider vector integer operations and gathered loads.
        AVX_512,   ///< Represents support for AVX-512 with 512-bit wide registers. Note: This support is marked as "Not Supported" when the system does not support AVX-512 operations.
        AVX_None   ///< No AVX
    };

    /**
     * @brief Enum class representing alignment sizes suitable for SIMD operations.
     * Choosing the correct alignment size is critical for maximizing data throughput
     * and minimizing cache line split issues during SIMD operations.
     */
    enum class SIMD_AlignmentSize {
        AlignmentSize_256 = 32, ///< Alignment size of 32 bytes, suitable for 256-bit AVX registers.
        AlignmentSize_512 = 64,  ///< Alignment size of 64 bytes, optimal not only for 512-bit AVX-512 registers but also for matching the typical cache line size of modern CPUs, enhancing cache utilization and reducing memory fetch overhead.
        AlignmentSize_None = 0 ///< No custom alignment size.
    };


    /**
    * @brief Class that encapsulates the policy for vectorization including the type of AVX
    *        and the alignment size.
    */
    class STLKR_SIMD {
    public:
        /**
         * @brief Constructs a VectorizationPolicy with specified AVX type and alignment size.
         * @param avxType The type of AVX support.
         * @param alignmentSize The alignment size in bits.
         */
        STLKR_SIMD(SIMD_AVXType avxType, SIMD_AlignmentSize alignmentSize) :
                            avxType(avxType), alignmentSize(alignmentSize) {}

        STLKR_SIMD() : avxType(SIMD_AVXType::AVX_None), alignmentSize(SIMD_AlignmentSize::AlignmentSize_None) {}

        /**
         * @brief Get the AVX type of this policy.
         * @return Returns the AVX type as a value from the SIMD_AVXType enum class.
         */
        SIMD_AVXType getAVXType() const {
            return avxType;
        }

        /**
         * @brief Get the alignment size of this policy.
         * @return Returns the alignment size as a value from the SIMD_AlignmentSize enum class.
         */
        SIMD_AlignmentSize getAlignmentSize() const {
            return alignmentSize;
        }

    private:
        SIMD_AVXType avxType; ///< The AVX type specified by this policy.
        SIMD_AlignmentSize alignmentSize; ///< The alignment size specified by this policy.
    };

}
#endif //STALKER_STLKR_SIMD_H
