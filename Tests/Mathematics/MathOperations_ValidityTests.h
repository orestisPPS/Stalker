#pragma once

#include "STLKR_TestBase.h"
#include <Stalker/Memory/Allocators.h>
#include <Stalker/Mathematics/Operations/MathOperations.h>
#include <Stalker/Mathematics/Random.h>
#include "TestUtility.h"

namespace STLKR_Tests {

    using namespace Stalker::Mathematics;
    using namespace Stalker::Memory;

    class MathOperations_ValidityTests : public STLKR_TestBase {
    public:


    explicit MathOperations_ValidityTests(size_t size = 1000, double tolerance = 1e-4) 
        : STLKR_TestBase("Math Operations Validity Tests"), _size(size), _tolerance(tolerance) {
            Random::setSeed(42); // Set a fixed seed for reproducibility
            // Random::setSeed(1); // Set a fixed seed for reproducibility
            // Constructor implementation
        }
    
    void runTest() override {
        printTitle("Math Operations Validity Tests", "-", ColourType::WHITE);
        _testAdd<double>();
        _testAdd<float>();
        _testAdd<int>();
        _testAdd<unsigned>();
        _testAdd<short>();

        _testSubtract<double>();
        _testSubtract<float>();
        _testSubtract<int>();
        _testSubtract<unsigned>();
        _testSubtract<short>();

        _testMultiply<double>();
        _testMultiply<float>();
        _testMultiply<int>();
        _testMultiply<unsigned>();
        _testMultiply<short>();

        // _testDivide<double>();
        // _testDivide<float>();
        // _testDivide<int>();
        // _testDivide<unsigned>();
        // _testDivide<short>();

        
        // _testScaledAdd();
        // _testSubtract();
        // _testScaledSubtract();
        // _testMultiply();
        // _testScaledMultiply();
        // _testDivide();
        // _testScaledDivide();
        // _testScale();
        
    }
    private:

    size_t _size;
    double _tolerance;

    
    template<typename T>
    void _testAdd() {
        printSubtitle("Addition " + TestUtility::getTypeString<T>(), ColourType::BARBIE_PINK);
        auto a = createAlignedUnique<T>(_size);
        auto b = createAlignedUnique<T>(_size);
        Random::uniform<T>(_size, a.get(), 0, 100);
        Random::uniform<T>(_size, b.get(), 0, 100);
        auto scalarA = Random::uniform<T>(0, 100);
        auto scalarB = Random::uniform<T>(0, 100);
        
        auto expected = createAlignedUnique<T>(_size);
        for (size_t i = 0; i < _size; i++) {
            expected.get()[i] = a.get()[i] + b.get()[i];
        }
        
        auto resultClassic = createAlignedUnique<T>(_size);
        MathOperations::add<T, ExecutionTraitClassic>(_size, a.get(), b.get(), resultClassic.get());
        TestUtility::compareVectors(resultClassic.get(), expected.get(), _size, "Classic");
        
        auto resultMeta = createAlignedUnique<T>(_size);
        MathOperations::add<T, ExecutionTraitUnrolledMeta<>>(_size, a.get(), b.get(), resultMeta.get());
        TestUtility::compareVectors(resultMeta.get(), expected.get(), _size, "Meta");
        
        auto resultSIMD = createAlignedUnique<T>(_size);
        MathOperations::add<T, ExecutionTraitSIMD<>>(_size, a.get(), b.get(), resultSIMD.get());
        TestUtility::compareVectors(resultSIMD.get(), expected.get(), _size, "SIMD");

        auto expectedScaled = createAlignedUnique<T>(_size);
        for (size_t i = 0; i < _size; i++) {
        expectedScaled.get()[i] = (a.get()[i] * scalarA) + (b.get()[i] * scalarB);
        }
        auto resultScaledClassic = createAlignedUnique<T>(_size);
        MathOperations::add<T, ExecutionTraitClassic>(_size, a.get(), b.get(), resultScaledClassic.get(), scalarA, scalarB);
        TestUtility::compareVectors(resultScaledClassic.get(), expectedScaled.get(), _size, "Classic Scaled");
        auto resultScaledMeta = createAlignedUnique<T>(_size);
        MathOperations::add<T, ExecutionTraitUnrolledMeta<>>(_size, a.get(), b.get(), resultScaledMeta.get(), scalarA, scalarB);
        TestUtility::compareVectors(resultScaledMeta.get(), expectedScaled.get(), _size, "Meta Scaled");
        auto resultScaledSIMD1 = createAlignedUnique<T>(_size);
        auto resultScaledSIMD2 = createAlignedUnique<T>(_size);
        MathOperations::add<T, ExecutionTraitSIMD<SIMDType::AVX2>>(_size, a.get(), b.get(), resultScaledSIMD1.get(), scalarA, scalarB);
        TestUtility::compareVectors<T>(resultScaledSIMD1.get(), expectedScaled.get(), _size, "SIMD AVX2 Scaled", _tolerance);
        MathOperations::add<T, ExecutionTraitSIMD<SIMDType::AVX512>>(_size, a.get(), b.get(), resultScaledSIMD2.get(), scalarA, scalarB);
        TestUtility::compareVectors<T>(resultScaledSIMD2.get(), expectedScaled.get(), _size, "SIMD AVX512 Scaled", _tolerance);
        auto solutionVector = std::vector<T>(_size);
        auto expectedVector = std::vector<T>(_size);
        for (size_t i = 0; i < _size; i++) {
            solutionVector[i] = resultScaledSIMD1.get()[i];
            expectedVector[i] = expectedScaled.get()[i];
            auto absDiff = std::fabs(solutionVector[i] - expectedVector[i]);
            if (absDiff > _tolerance) {
                // std::cout << "Difference at index " << i << ": " 
                //           << "Computed: " << solutionVector[i] 
                //           << ", Expected: " << expectedVector[i] 
                //           << ", AbsDiff: " << absDiff << std::endl;
            }
        }
    }

    template<typename T>
    void _testSubtract() {
        printSubtitle("Subtraction " + TestUtility::getTypeString<T>(), ColourType::BARBIE_PINK);
        auto a = createAlignedUnique<T>(_size);
        auto b = createAlignedUnique<T>(_size);
        Random::uniform<T>(_size, a.get(), 0, 100);
        Random::uniform<T>(_size, b.get(), 0, 100);
        auto scalarA = Random::uniform<T>(0, 100);
        auto scalarB = Random::uniform<T>(0, 100);
        
        auto expected = createAlignedUnique<T>(_size);
        for (size_t i = 0; i < _size; i++) {
            expected.get()[i] = a.get()[i] - b.get()[i];
        }
        
        auto resultClassic = createAlignedUnique<T>(_size);
        MathOperations::subtract<T, ExecutionTraitClassic>(_size, a.get(), b.get(), resultClassic.get());
        TestUtility::compareVectors(resultClassic.get(), expected.get(), _size, "Classic");
        
        auto resultMeta = createAlignedUnique<T>(_size);
        MathOperations::subtract<T, ExecutionTraitUnrolledMeta<>>(_size, a.get(), b.get(), resultMeta.get());
        TestUtility::compareVectors(resultMeta.get(), expected.get(), _size, "Meta");
        
        auto resultSIMD = createAlignedUnique<T>(_size);
        MathOperations::subtract<T, ExecutionTraitSIMD<>>(_size, a.get(), b.get(), resultSIMD.get());
        TestUtility::compareVectors(resultSIMD.get(), expected.get(), _size, "SIMD");

        auto expectedScaled = createAlignedUnique<T>(_size);
        for (size_t i = 0; i < _size; i++) {
            expectedScaled.get()[i] = a.get()[i] * scalarA - b.get()[i] * scalarB;
        }
        auto resultScaledClassic = createAlignedUnique<T>(_size);
        MathOperations::subtract<T, ExecutionTraitClassic>(_size, a.get(), b.get(), resultScaledClassic.get(), scalarA, scalarB);
        TestUtility::compareVectors(resultScaledClassic.get(), expectedScaled.get(), _size, "Scaled Classic");
        auto resultScaledMeta = createAlignedUnique<T>(_size);
        MathOperations::subtract<T, ExecutionTraitUnrolledMeta<>>(_size, a.get(), b.get(), resultScaledMeta.get(), scalarA, scalarB);
        TestUtility::compareVectors(resultScaledMeta.get(), expectedScaled.get(), _size, "Meta Scaled");
        auto resultScaledSIMD1 = createAlignedUnique<T>(_size);
        auto resultScaledSIMD2 = createAlignedUnique<T>(_size);
        MathOperations::subtract<T, ExecutionTraitSIMD<SIMDType::AVX2>>(_size, a.get(), b.get(), resultScaledSIMD1.get(), scalarA, scalarB);
        TestUtility::compareVectors<T>(resultScaledSIMD1.get(), expectedScaled.get(), _size, "SIMD AVX2 Scaled", _tolerance);
        MathOperations::subtract<T, ExecutionTraitSIMD<SIMDType::AVX512>>(_size, a.get(), b.get(), resultScaledSIMD2.get(), scalarA, scalarB);
        TestUtility::compareVectors<T>(resultScaledSIMD2.get(), expectedScaled.get(), _size, "SIMD AVX512 Scaled", _tolerance);
        auto solutionVector = std::vector<T>(_size);
        for (size_t i = 0; i < _size; i++) {
            solutionVector[i] = resultScaledSIMD1.get()[i];
        }
    }

    template<typename T>
    void _testMultiply() {
        printSubtitle("Multiplication " + TestUtility::getTypeString<T>(), ColourType::BARBIE_PINK);
        auto a = createAlignedUnique<T>(_size);
        auto b = createAlignedUnique<T>(_size);
        Random::uniform<T>(_size, a.get(), 0, 100);
        Random::uniform<T>(_size, b.get(), 0, 100);
        auto scalarA = Random::uniform<T>(0, 100);
        auto scalarB = Random::uniform<T>(0, 100);
        
        auto expected = createAlignedUnique<T>(_size);
        for (size_t i = 0; i < _size; i++) {
            expected.get()[i] = a.get()[i] * b.get()[i];
        }
        
        auto resultClassic = createAlignedUnique<T>(_size);
        MathOperations::multiply<T, ExecutionTraitClassic>(_size, a.get(), b.get(), resultClassic.get());
        TestUtility::compareVectors(resultClassic.get(), expected.get(), _size, "Classic");
        
        auto resultMeta = createAlignedUnique<T>(_size);
        MathOperations::multiply<T, ExecutionTraitUnrolledMeta<>>(_size, a.get(), b.get(), resultMeta.get());
        TestUtility::compareVectors(resultMeta.get(), expected.get(), _size, "Meta");
        
        auto resultSIMD = createAlignedUnique<T>(_size);
        MathOperations::multiply<T, ExecutionTraitSIMD<>>(_size, a.get(), b.get(), resultSIMD.get());
        TestUtility::compareVectors(resultSIMD.get(), expected.get(), _size, "SIMD");

        auto expectedScaled = createAlignedUnique<T>(_size);
        for (size_t i = 0; i < _size; i++) {
            expectedScaled.get()[i] = (a.get()[i] * scalarA) * (b.get()[i] * scalarB);
        }
        auto resultScaledClassic = createAlignedUnique<T>(_size);
        MathOperations::multiply<T, ExecutionTraitClassic>(_size, a.get(), b.get(), resultScaledClassic.get(), scalarA, scalarB);
        TestUtility::compareVectors(resultScaledClassic.get(), expectedScaled.get(), _size, "Scaled Classic");
        auto resultScaledMeta = createAlignedUnique<T>(_size);
        MathOperations::multiply<T, ExecutionTraitUnrolledMeta<>>(_size, a.get(), b.get(), resultScaledMeta.get(), scalarA, scalarB);
        TestUtility::compareVectors(resultScaledMeta.get(), expectedScaled.get(), _size, "Meta Scaled");
        auto resultScaledSIMD1 = createAlignedUnique<T>(_size);
        auto resultScaledSIMD2 = createAlignedUnique<T>(_size);
        MathOperations::multiply<T, ExecutionTraitSIMD<SIMDType::AVX2>>(_size, a.get(), b.get(), resultScaledSIMD1.get(), scalarA, scalarB);
        TestUtility::compareVectors<T>(resultScaledSIMD1.get(), expectedScaled.get(), _size, "SIMD AVX2 Scaled", _tolerance);
        // MathOperations::multiply<T, ExecutionTraitSIMD<SIMDType::AVX512>>(_size, a.get(), b.get(), resultScaledSIMD2.get(), scalarA, scalarB);
        // TestUtility::compareVectors<T>(resultScaledSIMD2.get(), expectedScaled.get(), _size, "SIMD AVX512 Scaled", _tolerance);

        auto solutionVector = std::vector<T>(_size);
        for (size_t i = 0; i < _size; i++) {
            solutionVector[i] = resultScaledSIMD1.get()[i];
        }
    }

    template<typename T>
    void _testScaledMultiply() {
        // Implementation of scaled multiplication tests
    }

    template<typename T>
    void _testDivide() {
        // Implementation of division tests
    }

    template<typename T>
    void _testScaledDivide() {
        // Implementation of scaled division tests
    }

    template<typename T>
    std::unique_ptr<T[]> _createArray(size_t size) {
        auto data = std::make_unique<T[]>(size);
        return data;
    }

    template <typename T>
    static std::unique_ptr<T[], decltype(&std::free)> _createAlignedPtr(size_t size, size_t alignment) {
        size_t padded_bytes = ((size * sizeof(T) + alignment - 1) / alignment) * alignment;
        void* rawPtr = std::aligned_alloc(alignment, padded_bytes);
        if (!rawPtr) throw std::bad_alloc();
        T* typedPtr = static_cast<T*>(rawPtr);
        assert(reinterpret_cast<uintptr_t>(typedPtr) % alignment == 0 && "Pointer is not properly aligned");
        return std::unique_ptr<T[], decltype(&std::free)>(typedPtr, &std::free);
    }


        

    };


} // namespace STLKR_Tests