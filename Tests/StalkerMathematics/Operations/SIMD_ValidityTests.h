#ifndef STALKER_MATHEMATICS_OPERATIONS_SIMD_VALIDITYTESTS_H
#define STALKER_MATHEMATICS_OPERATIONS_SIMD_VALIDITYTESTS_H

#include "../../STLKR_TestBase.h"
#include "../../TestUtility.h"
// #include "../../../DataStructures/StalkerVector/AVX2/MemoryTraits.h"
#include "../../../StalkerMathematics/Operations/SIMD/SIMDTypeTraits.h"

namespace STLKR_Tests {

    class SIMD_ValidityTests : public STLKR_TestBase {
    public:
        explicit SIMD_ValidityTests()
            : STLKR_TestBase("SIMD Validity Tests") {

            }

        void runTest() override {
            _testTypeTrait<double>();
            _testTypeTrait<float>();
            _testTypeTrait<int>();
            _testTypeTrait<short>();
            _testTypeTrait<unsigned>();
        }

    private:

        template<typename T>
        static void _testTypeTrait(){
            Printers::printSubtitle(TestUtility::getTypeString<T>(), ColourType::PATSIOURA_RED);
            constexpr unsigned expectedRegisterSize =
            std::is_same_v<T, double> ? DOUBLE_AVX2_REGISTER_SIZE :
            std::is_same_v<T, float>  ? FLOAT_AVX2_REGISTER_SIZE  :
            std::is_same_v<T, int>    ? INT_AVX2_REGISTER_SIZE    :
            std::is_same_v<T, short>  ? SHORT_AVX2_REGISTER_SIZE  :
            std::is_same_v<T, unsigned> ? UNSIGNED_AVX2_REGISTER_SIZE : 0;
            constexpr unsigned expectedElementsPerCacheLine = 64 / sizeof(T);
            constexpr unsigned expectedCacheLinesProcessed = (UnrollFactorSIMD * expectedRegisterSize) / expectedElementsPerCacheLine;
            constexpr unsigned expectedBlockSize = expectedRegisterSize * UnrollFactorSIMD;
            
            TestUtility::compareValues<unsigned>(SIMDTypeTraits<T, SIMDType::AVX2>::RegisterSize, expectedRegisterSize, "Register Size");
            TestUtility::compareValues<unsigned>(SIMDTypeTraits<T, SIMDType::AVX2>::ElementsPerCacheLine, expectedElementsPerCacheLine, "Elements Per Cache Line");
            TestUtility::compareValues<unsigned>(SIMDTypeTraits<T, SIMDType::AVX2>::CacheLinesProcessed, expectedCacheLinesProcessed, "Cache Lines Processed");
            TestUtility::compareValues<unsigned>(SIMDTypeTraits<T, SIMDType::AVX2>::BlockSize, expectedBlockSize, "Block Size");
            
            static_assert(SIMDTypeTraits<T, SIMDType::AVX2>::RegisterSize == expectedRegisterSize, "Register Size is not correct");
            static_assert(SIMDTypeTraits<T, SIMDType::AVX2>::ElementsPerCacheLine == expectedElementsPerCacheLine, "Elements Per Cache Line is not correct");
            static_assert(SIMDTypeTraits<T, SIMDType::AVX2>::CacheLinesProcessed == expectedCacheLinesProcessed, "Cache Lines Processed is not correct");
            static_assert(SIMDTypeTraits<T, SIMDType::AVX2>::BlockSize == expectedBlockSize, "Block Size is not correct");
        }

    };

} // namespace STLKR_Tests



#endif // STALKER_MATHEMATICS_OPERATIONS_SIMD_VALIDITYTESTS_H