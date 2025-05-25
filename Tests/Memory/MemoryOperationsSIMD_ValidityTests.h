#pragma once

#include "../STLKR_TestBase.h"
#include "../TestUtility.h"
#include <Stalker/Memory/SIMD/MemoryOperationsSIMDAVX2.h>
#include <Stalker/Memory/SIMD/MemoryOperationsSIMDAVX512.h>

namespace STLKR_Tests {
    using namespace Stalker::Memory::SIMD;
    
    template<size_t Size = 32>
    class MemoryOperationsSIMD_ValidityTests : public STLKR_TestBase {
    public:
        explicit MemoryOperationsSIMD_ValidityTests() : STLKR_TestBase("SIMD Memory Operations Validity Tests") { }

        void runTest() override {
            _testAllMemoryOperations();
        }

    private:

        static void _testAllMemoryOperations(){
            printTitle("AVX2", "-", ColourType::WHITE);
            _testSIMDMemory<double,   SIMDType::AVX2>();
            _testSIMDMemory<float,    SIMDType::AVX2>();
            _testSIMDMemory<int,      SIMDType::AVX2>();
            _testSIMDMemory<short,    SIMDType::AVX2>();
            _testSIMDMemory<unsigned, SIMDType::AVX2>();
            printTitle("AVX512", "-", ColourType::WHITE);
            _testSIMDMemory<double, SIMDType::AVX512>();
            _testSIMDMemory<float, SIMDType::AVX512>();
            _testSIMDMemory<int, SIMDType::AVX512>();
            _testSIMDMemory<short, SIMDType::AVX512>();
            _testSIMDMemory<unsigned, SIMDType::AVX512>();
        }


        template<typename T, SIMDType SimdT>
        static void _testSIMDMemory(){
            printSubtitle(TestUtility::getTypeString<T>(), ColourType::PATSIOURA_RED);

            auto expectedZero = _createEmptyAlignedPtr<T>(Size, 64);
            auto expectedSetValue = _createAlignedPtr<T>(Size, 64, 1);
            auto expectedCopy = _createAlignedPtr<T>(Size, 64);

            auto result = static_cast<T*>(_mm_malloc(Size * sizeof(T), 64));

            MemoryOperationsSIMD<T, SimdT>::setZero(result, Size);
            TestUtility::compareVectors(expectedZero, result, Size, "Set Zero");

            MemoryOperationsSIMD<T, SimdT>::setValue(result, static_cast<T>(1), Size);
            TestUtility::compareVectors(expectedSetValue, result, Size, "Set Value");
            
            MemoryOperationsSIMD<T, SimdT>::copy(expectedCopy, result, Size);
            TestUtility::compareVectors<T>(expectedCopy, result, Size, "Copy");

            _freeAlignedArray(expectedZero);
            _freeAlignedArray(expectedSetValue);
            _freeAlignedArray(expectedCopy);
            _freeAlignedArray(result);
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