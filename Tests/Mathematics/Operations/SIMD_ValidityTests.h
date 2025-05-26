#ifndef STALKER_MATHEMATICS_OPERATIONS_SIMD_VALIDITYTESTS_H
#define STALKER_MATHEMATICS_OPERATIONS_SIMD_VALIDITYTESTS_H

#include "../../STLKR_TestBase.h"
#include "../../TestUtility.h"
// #include "../../../DataStructures/StalkerVector/AVX2/MemoryTraits.h"
#include <Stalker/Mathematics/Operations/MathOperationsSIMD.h>
#include <Stalker/Mathematics/Operations/SIMD/MathOperationsSIMDAVX2.h>
#include <Stalker/Mathematics/Operations/SIMD/MathOperationsSIMDAVX512.h>
// #include "../../../StalkerMathematics/Operations/SIMD/SIMDMathOperations.h"

namespace STLKR_Tests {
    using namespace Stalker::Mathematics;
    
    template<size_t Size = 32>
    class SIMD_ValidityTests : public STLKR_TestBase {
    
        public:
        explicit SIMD_ValidityTests() : STLKR_TestBase("MathOperationsSIMD Validity Tests") {

            }

        void runTest() override {
            _testAllMathOperations();
        }

    private:

        static void _testAllMathOperations(){
            printTitle("AVX2", "-", ColourType::WHITE);
            _testSIMDMath<double, SIMDType::AVX2>();
            _testSIMDMath<float, SIMDType::AVX2>();
            _testSIMDMath<int, SIMDType::AVX2>();
            _testSIMDMath<short, SIMDType::AVX2>();
            _testSIMDMath<unsigned, SIMDType::AVX2>();
            printTitle("AVX512", "-", ColourType::WHITE);
            _testSIMDMath<double, SIMDType::AVX512>();
            _testSIMDMath<float, SIMDType::AVX512>();
            _testSIMDMath<int, SIMDType::AVX512>();
            _testSIMDMath<unsigned, SIMDType::AVX512>();
            _testSIMDMath<short, SIMDType::AVX512>();
        }


        template<typename T, SIMDType Type>
        static void _testSIMDMath(){
            printSubtitle(TestUtility::getTypeString<T>(), ColourType::PATSIOURA_RED);

            T valA = static_cast<T>(std::is_floating_point<T>::value ? 1.5 : 2);
            T valB = static_cast<T>(std::is_floating_point<T>::value ? 2.5 : 3);
            auto a = _createAlignedPtr<T>(Size, 64, valA);
            auto b = _createAlignedPtr<T>(Size, 64, valB);

            // Compute expected results for each type
            auto expectedAdd = _createAlignedPtr<T>(Size, 64, valA + valB);
            auto expectedSubtract = _createAlignedPtr<T>(Size, 64, valA - valB);
            auto expectedMultiply = _createAlignedPtr<T>(Size, 64, valA * valB);
            auto result = static_cast<T*>(_mm_malloc(Size * sizeof(T), 64));



            Stalker::Mathematics::SIMD::SIMDMathOperations<T, Type>::add(a, b, result, Size);
            TestUtility::compareVectors(result, expectedAdd, Size, "Add");
            // Stalker::Mathematics::SIMD::SIMDMathOperations<T, Type>::add(a, b, result, Size, 1.0, 1.0);
            // TestUtility::compareVectors(result, expectedAdd, Size, "Add with scale");

            Stalker::Mathematics::SIMD::SIMDMathOperations<T, Type>::subtract(a, b, result, Size);
            TestUtility::compareVectors(result, expectedSubtract, Size, "Subtract");
            // Stalker::Mathematics::SIMD::SIMDMathOperations<T, Type>::subtract(a, b, result, Size, 1.0, 1.0);
            // TestUtility::compareVectors(result, expectedSubtract, Size, "Subtract with scale");

            Stalker::Mathematics::SIMD::SIMDMathOperations<T, Type>::multiply(a, b, result, Size);
            TestUtility::compareVectors(result, expectedMultiply, Size, "Multiply");
            // Stalker::Mathematics::SIMD::SIMDMathOperations<T, Type>::multiply(a, b, result, Size, 1.0, 1.0);
            // TestUtility::compareVectors(result, expectedMultiply, Size, "Multiply with scale");


            _freeAlignedArray(a);
            _freeAlignedArray(b);
            _freeAlignedArray(expectedAdd);
            _freeAlignedArray(expectedSubtract);
            _freeAlignedArray(expectedMultiply);
            _freeAlignedArray(result);

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