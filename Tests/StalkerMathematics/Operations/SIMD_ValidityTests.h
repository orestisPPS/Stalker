#ifndef STALKER_MATHEMATICS_OPERATIONS_SIMD_VALIDITYTESTS_H
#define STALKER_MATHEMATICS_OPERATIONS_SIMD_VALIDITYTESTS_H

#include "../../STLKR_TestBase.h"
#include "../../TestUtility.h"
// #include "../../../DataStructures/StalkerVector/AVX2/MemoryTraits.h"
#include "../../../StalkerMathematics/Operations/SIMD/SIMDTypeTraits.h"
#include "../../../StalkerMathematics/Operations/SIMD/SIMDMemoryOperations.h"
#include "../../../StalkerMathematics/Operations/SIMD/SIMDMathOperations.h"

namespace STLKR_Tests {
    
    template<size_t Size = 32>
    class SIMD_ValidityTests : public STLKR_TestBase {
    public:
        explicit SIMD_ValidityTests() : STLKR_TestBase("SIMD Validity Tests") {

            }

        void runTest() override {
            _testAllTypeTraits();
            _testAllMemoryOperations();
            _testAllMathOperations();
        }

    private:

        static void _testAllTypeTraits(){
            Printers::printTitle("Type Traits", "-", ColourType::WHITE);
            _testTypeTrait<double>();
            _testTypeTrait<float>();
            _testTypeTrait<int>();
            _testTypeTrait<short>();
            _testTypeTrait<unsigned>();
        }

        static void _testAllMemoryOperations(){
            Printers::printTitle("Memory Operations", "-", ColourType::WHITE);
            _testSIMDMemory<double>();
            _testSIMDMemory<float>();
            _testSIMDMemory<int>();
            _testSIMDMemory<short>();
            _testSIMDMemory<unsigned>();
        }

        static void _testAllMathOperations(){
            Printers::printTitle("Math Operations", "-", ColourType::WHITE);
            _testSIMDMath<double>();
            _testSIMDMath<float>();
            _testSIMDMath<int>();
            _testSIMDMath<short>();
            _testSIMDMath<unsigned>();
        }

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

        template<typename T>
        static void _testSIMDMemory(){
            Printers::printSubtitle(TestUtility::getTypeString<T>(), ColourType::PATSIOURA_RED);

            auto expectedZero = _createEmptyAlignedPtr<T>(Size, 64);
            auto expectedSetValue = _createAlignedPtr<T>(Size, 64, 1);
            auto expectedCopy = _createAlignedPtr<T>(Size, 64);

            auto result = static_cast<T*>(_mm_malloc(Size * sizeof(T), 64));

            SIMDMemoryOperations<T, SIMDType::AVX2>::setZero(result, Size);
            TestUtility::compareVectors(expectedZero, result, Size, "Set Zero");

            SIMDMemoryOperations<T, SIMDType::AVX2>::setValue(result, static_cast<T>(1), Size);
            TestUtility::compareVectors(expectedSetValue, result, Size, "Set Value");
            
            SIMDMemoryOperations<T, SIMDType::AVX2>::copy(expectedCopy, result, Size);
            TestUtility::compareVectors<T>(expectedCopy, result, Size, "Copy");

            _freeAlignedArray(expectedZero);
            _freeAlignedArray(expectedSetValue);
            _freeAlignedArray(expectedCopy);
            _freeAlignedArray(result);
        }

        template<typename T>
        static void _testSIMDMath(){
            Printers::printSubtitle(TestUtility::getTypeString<T>(), ColourType::PATSIOURA_RED);

            // auto v1 = _createAlignedArray<T, 8>(1);
            // auto v2 = _createAlignedArray<T, 8>(0);

        }

        template <typename T>
        static std::array<T, Size> _createAlignedArray(T value = 0) {
            std::array<T, Size> array;
            for (size_t i = 0; i < Size; i++){
                if (value == 0)
                    array[i] = static_cast<T>(i);
                else
                    array[i] = value;
            }
            return array;
        }
        
        template <typename T>
        static std::array<T, Size> _createEmptyAlignedArray() {
            alignas(64) std::array<T, Size> array;
            for (size_t i = 0; i < Size; i++){
                array[i] = 0;
            }
            return array;
        }


        template <typename T>
        static auto _createAlignedPtr(size_t size, size_t alignment, T value = 0) {
            T* ptr = static_cast<T*>(_mm_malloc(size * sizeof(T), alignment));
            for (size_t i = 0; i < size; i++) {
                ptr[i] = (value == 0) ? static_cast<T>(i) : value;
            }
            return ptr;
        }

        template <typename T>
        static auto _createEmptyAlignedPtr(size_t size, size_t alignment) {
            T* ptr = static_cast<T*>(_mm_malloc(size * sizeof(T), alignment));
            for (size_t i = 0; i < size; i++) {
                ptr[i] = 0;
            }
            return ptr;
        }

        template <typename T>
        static void _freeAlignedArray(T* ptr) {
            _mm_free(ptr);
        }

        

    };

} // namespace STLKR_Tests



#endif // STALKER_MATHEMATICS_OPERATIONS_SIMD_VALIDITYTESTS_H