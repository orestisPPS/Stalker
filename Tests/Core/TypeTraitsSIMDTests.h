#pragma once

#include "../STLKR_TestBase.h"
#include "../TestUtility.h"
#include <Stalker/Core/Traits/TypeTraits/SIMD/TypeTraitsSIMDAVX2.h>
#include <Stalker/Core/Traits/TypeTraits/SIMD/TypeTraitsSIMDAVX512.h>

namespace STLKR_Tests {
    
    class TypeTraitsSIMDTests : public STLKR_TestBase {
    public:
        explicit TypeTraitsSIMDTests() : STLKR_TestBase("SIMD Traits Validity Tests") {

        }

        void runTest() override {
            _testAllTypeTraits();
        }

    private:

        static void _testAllTypeTraits(){
            printTitle("AVX2", "-", T_Color::WHITE);
            _testTypeTraitAVX2<double>();
            _testTypeTraitAVX2<float>();
            _testTypeTraitAVX2<int>();
            _testTypeTraitAVX2<short>();
            _testTypeTraitAVX2<unsigned>();
            printTitle("AVX512", "-", T_Color::WHITE);
            _testTypeTraitAVX512<double>();
            _testTypeTraitAVX512<float>();
            _testTypeTraitAVX512<int>();
            _testTypeTraitAVX512<short>();
            _testTypeTraitAVX512<unsigned>();
        }



        template<typename T, T_SIMD, T_SIMD SimdT>
        static void _testTypeTrait(){
            if constexpr (SimdT == T_SIMD::AVX2) {
                _testTypeTraitAVX2<T>();
            } else if constexpr (SimdT == T_SIMD::AVX512) {
                _testTypeTraitAVX512<T>();
            }
        }

        template<typename U>
        static void _testTypeTraitAVX2() {
            printSubtitle(TestUtility::getTypeString<U>(), T_Color::PATSIOURA_RED);

            constexpr unsigned expectedRegisterSize =
            std::is_same_v<U, double>   ? 4  :
            std::is_same_v<U, float>    ? 8  :
            std::is_same_v<U, int>      ? 8  :
            std::is_same_v<U, short>    ? 16 :
            std::is_same_v<U, unsigned> ? 8  :
            0;
            constexpr unsigned expectedElementsPerCacheLine = STALKER_PLATFORM_CACHE_LINE_SIZE / sizeof(U);
            constexpr unsigned expectedCacheLinesProcessed = (STALKER_UNROLL_FACTOR * expectedRegisterSize) / expectedElementsPerCacheLine;
            constexpr unsigned expectedBlockSize = expectedRegisterSize * STALKER_UNROLL_FACTOR;

            TestUtility::compareValues<unsigned>(TypeTraitsSIMD<U, T_SIMD::AVX2>::RegisterSize(), expectedRegisterSize, "Register Size");
            static_assert(TypeTraitsSIMD<U, T_SIMD::AVX2>::RegisterSize() == expectedRegisterSize, "Register Size is not correct");
            TestUtility::compareValues<unsigned>(TypeTraitsSIMD<U, T_SIMD::AVX2>::ElementsPerCacheLine(), expectedElementsPerCacheLine, "Elements Per Cache Line");
            TestUtility::compareValues<unsigned>(TypeTraitsSIMD<U, T_SIMD::AVX2>::CacheLinesProcessed(), expectedCacheLinesProcessed, "Cache Lines Processed");
            TestUtility::compareValues<unsigned>(TypeTraitsSIMD<U, T_SIMD::AVX2>::BlockSize(), expectedBlockSize, "Block Size");

            static_assert(TypeTraitsSIMD<U, T_SIMD::AVX2>::ElementsPerCacheLine() == expectedElementsPerCacheLine, "Elements Per Cache Line is not correct");
            static_assert(TypeTraitsSIMD<U, T_SIMD::AVX2>::CacheLinesProcessed() == expectedCacheLinesProcessed, "Cache Lines Processed is not correct");
            static_assert(TypeTraitsSIMD<U, T_SIMD::AVX2>::BlockSize() == expectedBlockSize, "Block Size is not correct");
        }

        template<typename U>
        static void _testTypeTraitAVX512() {
            printSubtitle(TestUtility::getTypeString<U>(), T_Color::PATSIOURA_RED);

            constexpr unsigned expectedRegisterSize =
            std::is_same_v<U, double>   ? 8  :
            std::is_same_v<U, float>    ? 16 :
            std::is_same_v<U, int>      ? 16 :
            std::is_same_v<U, short>    ? 32 :
            std::is_same_v<U, unsigned> ? 16 :
            0;
            constexpr unsigned expectedElementsPerCacheLine = STALKER_PLATFORM_CACHE_LINE_SIZE / sizeof(U);
            constexpr unsigned expectedCacheLinesProcessed = (STALKER_UNROLL_FACTOR * expectedRegisterSize) / expectedElementsPerCacheLine;
            constexpr unsigned expectedBlockSize = expectedRegisterSize * STALKER_UNROLL_FACTOR;

            TestUtility::compareValues<unsigned>(TypeTraitsSIMD<U, T_SIMD::AVX512>::RegisterSize(), expectedRegisterSize, "Register Size");
            static_assert(TypeTraitsSIMD<U, T_SIMD::AVX512>::RegisterSize() == expectedRegisterSize, "Register Size is not correct");
            TestUtility::compareValues<unsigned>(TypeTraitsSIMD<U, T_SIMD::AVX512>::ElementsPerCacheLine(), expectedElementsPerCacheLine, "Elements Per Cache Line");
            TestUtility::compareValues<unsigned>(TypeTraitsSIMD<U, T_SIMD::AVX512>::CacheLinesProcessed(), expectedCacheLinesProcessed, "Cache Lines Processed");
            TestUtility::compareValues<unsigned>(TypeTraitsSIMD<U, T_SIMD::AVX512>::BlockSize(), expectedBlockSize, "Block Size");

            static_assert(TypeTraitsSIMD<U, T_SIMD::AVX512>::ElementsPerCacheLine() == expectedElementsPerCacheLine, "Elements Per Cache Line is not correct");
            static_assert(TypeTraitsSIMD<U, T_SIMD::AVX512>::CacheLinesProcessed() == expectedCacheLinesProcessed, "Cache Lines Processed is not correct");
            static_assert(TypeTraitsSIMD<U, T_SIMD::AVX512>::BlockSize() == expectedBlockSize, "Block Size is not correct");
        }

    };

} // namespace STLKR_Tests