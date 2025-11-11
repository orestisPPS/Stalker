#pragma once

#include "STLKR_TestBase.h"
#include <Stalker/Core/Units.h>
#include <Stalker/Memory/Allocators.h>
#include <Stalker/Mathematics/Operations/MathOperations.h>
#include <Stalker/Threading/ThreadingTraits.h>
#include <Stalker/Mathematics/Random.h>
#include "TestUtility.h"

namespace STLKR_Tests {

    using namespace Stalker::Mathematics;
    using namespace Stalker::Memory;
    using namespace Stalker::Threading;

    class MathOperations_ValidityTests : public STLKR_TestBase {
    public:

    explicit MathOperations_ValidityTests(size_t size = 10000, double tolerance = 1e-8)  
                : STLKR_TestBase("MathOperations"), _size(size), _tolerance(tolerance) {
            setPath(_testName + "Logs", getPath(_testName) + "/logs/" + _testName);
            Random::setSeed(42); // Set a fixed seed for reproducibility
        }
    
    void runTest() override {
        printTitle("Math Operations Validity Tests", "-", T_Color::WHITE);
        _testAdd<double>();
        _testAdd<float>();
        _testAdd<int>();
        _testAdd<unsigned>();
        _testAdd<short>();

        _testAxpy<double>();
        _testAxpy<float>();
        _testAxpy<int>();
        _testAxpy<unsigned>();
        _testAxpy<short>();

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

        _testSum<double>();
        _testSum<float>();
        _testSum<int>();
        _testSum<unsigned>();
        _testSum<short>();

        _testScale<double>();
        _testScale<float>();
        _testScale<int>();
        _testScale<unsigned>();
        _testScale<short>(); 

        _testAddConstant<double>();
        _testAddConstant<float>();
        _testAddConstant<int>();
        _testAddConstant<unsigned>();
        _testAddConstant<short>();  

        _testDotProduct<double>();
        _testDotProduct<float>();
        _testDotProduct<int>();
        _testDotProduct<unsigned>();
        _testDotProduct<short>();  

    }
    private:

    size_t _size;
    double _tolerance;

    
    template<typename T>
    void _testAdd() {

        printSubtitle("Addition " + typeToString<T>(), T_Color::BARBIE_PINK);
        auto typeName = typeToString<T>();
        auto unit = Stalker::Core::TimeUnit::nanoseconds;


        // auto logs = _logs.

        auto a = createAlignedVector<T>(_size);
        auto b = createAlignedVector<T>(_size);
        Random::uniform<T>(_size, a.data(), 0, 100);
        Random::uniform<T>(_size, b.data(), 0, 100);
        auto scalarA = Random::uniform<T>(0, 100);
        auto scalarB = Random::uniform<T>(0, 100);
        
        auto expected = createAlignedVector<T>(_size);
        auto expectedScaled = createAlignedVector<T>(_size);
        for (size_t i = 0; i < _size; i++) {
            expected[i] = a[i] + b[i];
            expectedScaled[i] = (a[i] * scalarA) + (b[i] * scalarB);
        }
        
        {
            auto resClassic = createAlignedVector<T>(_size);
            MathOperations::add<T, ExecutionTraitClassic<>>(_size, a.data(), b.data(), resClassic.data());
            TestUtility::compareVectors(resClassic.data(), expected.data(), _size, "Classic");
        }

        {
            auto resMeta = createAlignedVector<T>(_size);
            MathOperations::add<T, ExecutionTraitUnrolled<100>>(_size, a.data(), b.data(), resMeta.data());
            TestUtility::compareVectors(resMeta.data(), expected.data(), _size, "Meta");
        }
        
        {
            auto resSIMD1 = createAlignedVector<T>(_size);
            MathOperations::add<T, ExecutionTraitSIMD<T_SIMD::AVX2, 1>>(_size, a.data(), b.data(), resSIMD1.data());
            TestUtility::compareVectors<T>(resSIMD1.data(), expected.data(), _size, "SIMD AVX2", _tolerance);
        }

        {
            auto resSIMD2 = createAlignedVector<T>(_size);
            MathOperations::add<T, ExecutionTraitSIMD<T_SIMD::AVX512, 1>>(_size, a.data(), b.data(), resSIMD2.data());
            TestUtility::compareVectors<T>(resSIMD2.data(), expected.data(), _size, "SIMD AVX512", _tolerance);
        }

        {
            auto resScaledClassic = createAlignedVector<T>(_size);
            MathOperations::add<T, ExecutionTraitClassic<>>(_size, a.data(), b.data(), resScaledClassic.data(), scalarA, scalarB);
            TestUtility::compareVectors(resScaledClassic.data(), expectedScaled.data(), _size, "Classic Scaled");
        }

        {
            auto resScaledMeta = createAlignedVector<T>(_size);
            MathOperations::add<T, ExecutionTraitUnrolled<16>>(_size, a.data(), b.data(), resScaledMeta.data(), scalarA, scalarB);
            TestUtility::compareVectors(resScaledMeta.data(), expectedScaled.data(), _size, "Meta Scaled");
        }

        {
            auto resScaledSIMD1 = createAlignedVector<T>(_size);
            MathOperations::add<T, ExecutionTraitSIMD<T_SIMD::AVX2, 2>>(_size, a.data(), b.data(), resScaledSIMD1.data(), scalarA, scalarB);
            TestUtility::compareVectors<T>(resScaledSIMD1.data(), expectedScaled.data(), _size, "SIMD AVX2 Scaled", _tolerance);
        }

        {
            auto resScaledSIMD2 = createAlignedVector<T>(_size);
            MathOperations::add<T, ExecutionTraitSIMD<T_SIMD::AVX512, 2>>(_size, a.data(), b.data(), resScaledSIMD2.data(), scalarA, scalarB);
            TestUtility::compareVectors<T>(resScaledSIMD2.data(), expectedScaled.data(), _size, "SIMD AVX512 Scaled", _tolerance);
        }

        ThreadTraitSTDThread ThreadTrait = ThreadTraitSTDThread(1);
        auto nThreads = ThreadTrait.getNumThreads();
        auto nThreadsStr = std::to_string(nThreads);

        {
            auto resClassicThreaded = createAlignedVector<T>(_size);
            MathOperations::add<T, ThreadTraitSTDThread, ExecutionTraitClassic<>>(ThreadTrait, _size, a.data(), b.data(), resClassicThreaded.data());
            TestUtility::compareVectors(resClassicThreaded.data(), expected.data(), _size, "Classic Threaded");
        }

        {
            auto resMetaThreaded = createAlignedVector<T>(_size);
            MathOperations::add<T, ThreadTraitSTDThread, ExecutionTraitUnrolled<16>>(ThreadTrait, _size, a.data(), b.data(), resMetaThreaded.data());
            TestUtility::compareVectors(resMetaThreaded.data(), expected.data(), _size, "Meta Threaded");
        }

        {
            auto resMetaThreaded = createAlignedVector<T>(_size);
            MathOperations::add<T, ThreadTraitSTDThread, ExecutionTraitUnrolled<16>>(ThreadTrait, _size, a.data(), b.data(), resMetaThreaded.data());
            TestUtility::compareVectors(resMetaThreaded.data(), expected.data(), _size, "Meta Threaded");
        }

        {
            auto resSIMD1Threaded = createAlignedVector<T>(_size);
            MathOperations::add<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2>>(ThreadTrait, _size, a.data(), b.data(), resSIMD1Threaded.data());
            TestUtility::compareVectors<T>(resSIMD1Threaded.data(), expected.data(), _size, "SIMD AVX2 Threaded", _tolerance);
        }

        {
            auto resSIMD2Threaded = createAlignedVector<T>(_size);
            MathOperations::add<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512, 1>>(ThreadTrait, _size, a.data(), b.data(), resSIMD2Threaded.data());
            TestUtility::compareVectors<T>(resSIMD2Threaded.data(), expected.data(), _size, "SIMD AVX512 Threaded", _tolerance);
        }

        {
            auto resScaledClassicThreaded = createAlignedVector<T>(_size);
            MathOperations::add<T, ThreadTraitSTDThread, ExecutionTraitClassic<>>(ThreadTrait, _size, a.data(), b.data(), resScaledClassicThreaded.data(), scalarA, scalarB);
            TestUtility::compareVectors(resScaledClassicThreaded.data(), expectedScaled.data(), _size, "Classic Scaled Threaded");
        }

        {
            auto resScaledMetaThreaded = createAlignedVector<T>(_size);
            MathOperations::add<T, ThreadTraitSTDThread, ExecutionTraitUnrolled<16>>(ThreadTrait, _size, a.data(), b.data(), resScaledMetaThreaded.data(), scalarA, scalarB);
            TestUtility::compareVectors(resScaledMetaThreaded.data(), expectedScaled.data(), _size, "Meta Scaled Threaded");
        }

        {
            auto resScaledSIMD1Threaded = createAlignedVector<T>(_size);
            MathOperations::add<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2>>(ThreadTrait, _size, a.data(), b.data(), resScaledSIMD1Threaded.data(), scalarA, scalarB);
            TestUtility::compareVectors<T>(resScaledSIMD1Threaded.data(), expectedScaled.data(), _size, "SIMD AVX2 Scaled Threaded", _tolerance);
        }

        {
            auto resScaledSIMD2Threaded = createAlignedVector<T>(_size);
            MathOperations::add<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512>>(ThreadTrait, _size, a.data(), b.data(), resScaledSIMD2Threaded.data(), scalarA, scalarB);
            TestUtility::compareVectors<T>(resScaledSIMD2Threaded.data(), expectedScaled.data(), _size, "SIMD AVX512 Scaled Threaded", _tolerance);
        }

    }

    template<typename T>
    void _testAxpy() {

        printSubtitle("Axpy " + typeToString<T>(), T_Color::BARBIE_PINK);
        auto typeName = typeToString<T>();
        auto unit = Stalker::Core::TimeUnit::nanoseconds;


        // auto logs = _logs.

        auto a = createAlignedVector<T>(_size);
        auto b = createAlignedVector<T>(_size);
        Random::uniform<T>(_size, a.data(), 0, 100);
        Random::uniform<T>(_size, b.data(), 0, 100);
        auto scalarA = Random::uniform<T>(0, 100);
        
        auto expected = createAlignedVector<T>(_size);
        auto expectedScaled = createAlignedVector<T>(_size);
        for (size_t i = 0; i < _size; i++) {
            expected[i] = (a[i] * scalarA) + b[i];
        }
        
        {
            auto resClassic = createAlignedVector<T>(_size);
            MathOperations::axpy<T, ExecutionTraitClassic<>>(_size, a.data(), b.data(), resClassic.data(), scalarA);
            TestUtility::compareVectors(resClassic.data(), expected.data(), _size, "Classic");
        }

        {
            auto resMeta = createAlignedVector<T>(_size);
            MathOperations::axpy<T, ExecutionTraitUnrolled<100>>(_size, a.data(), b.data(), resMeta.data(), scalarA);
            TestUtility::compareVectors(resMeta.data(), expected.data(), _size, "Meta");
        }
        
        {
            auto resSIMD1 = createAlignedVector<T>(_size);
            MathOperations::axpy<T, ExecutionTraitSIMD<T_SIMD::AVX2, 1>>(_size, a.data(), b.data(), resSIMD1.data(), scalarA);
            TestUtility::compareVectors<T>(resSIMD1.data(), expected.data(), _size, "SIMD AVX2", _tolerance);
        }

        {
            auto resSIMD2 = createAlignedVector<T>(_size);
            MathOperations::axpy<T, ExecutionTraitSIMD<T_SIMD::AVX512, 1>>(_size, a.data(), b.data(), resSIMD2.data(), scalarA);
            TestUtility::compareVectors<T>(resSIMD2.data(), expected.data(), _size, "SIMD AVX512", _tolerance);
        }


        ThreadTraitSTDThread ThreadTrait = ThreadTraitSTDThread();
        auto nThreads = ThreadTrait.getNumThreads();
        auto nThreadsStr = std::to_string(nThreads);

        {
            auto resClassic = createAlignedVector<T>(_size);
            MathOperations::axpy<T, ThreadTraitSTDThread, ExecutionTraitClassic<>>(ThreadTrait, _size, a.data(), b.data(), resClassic.data(), scalarA);
            TestUtility::compareVectors(resClassic.data(), expected.data(), _size, "Classic Threaded (" + nThreadsStr + " threads)");
        }

        {
            auto resMeta = createAlignedVector<T>(_size);
            MathOperations::axpy<T, ThreadTraitSTDThread, ExecutionTraitUnrolled<100>>(ThreadTrait, _size, a.data(), b.data(), resMeta.data(), scalarA);
            TestUtility::compareVectors(resMeta.data(), expected.data(), _size, "Meta Threaded (" + nThreadsStr + " threads)");
        }
        
        {
            auto resSIMD1 = createAlignedVector<T>(_size);
            MathOperations::axpy<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2, 1>>(ThreadTrait, _size, a.data(), b.data(), resSIMD1.data(), scalarA);
            TestUtility::compareVectors<T>(resSIMD1.data(), expected.data(), _size, "SIMD AVX2 Threaded (" + nThreadsStr + " threads)", _tolerance);
        }

        {
            auto resSIMD2 = createAlignedVector<T>(_size);
            MathOperations::axpy<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512, 1>>(ThreadTrait, _size, a.data(), b.data(), resSIMD2.data(), scalarA);
            TestUtility::compareVectors<T>(resSIMD2.data(), expected.data(), _size, "SIMD AVX512 Threaded (" + nThreadsStr + " threads)", _tolerance);
        }
    }

    template<typename T>
    void _testSubtract() {
        printSubtitle("Subtraction " + typeToString<T>(), T_Color::BARBIE_PINK);
        auto a = createAlignedVector<T>(_size);
        auto b = createAlignedVector<T>(_size);
        Random::uniform<T>(_size, a.data(), 0, 100);
        Random::uniform<T>(_size, b.data(), 0, 100);
        auto scalarA = Random::uniform<T>(0, 100);
        auto scalarB = Random::uniform<T>(0, 100);
        
        auto expected = createAlignedVector<T>(_size);
        auto expectedScaled = createAlignedVector<T>(_size);
        for (size_t i = 0; i < _size; i++) {
            expected[i] = a[i] - b[i];
            expectedScaled[i] = (a[i] * scalarA) - (b[i] * scalarB);
        }
        
        auto resClassic = createAlignedVector<T>(_size);
        MathOperations::subtract<T, ExecutionTraitClassic<>>(_size, a.data(), b.data(), resClassic.data());
        TestUtility::compareVectors(resClassic.data(), expected.data(), _size, "Classic");

        auto resMeta = createAlignedVector<T>(_size);
        MathOperations::subtract<T, ExecutionTraitUnrolled<>>(_size, a.data(), b.data(), resMeta.data());
        TestUtility::compareVectors(resMeta.data(), expected.data(), _size, "Meta");
        
        auto resSIMD = createAlignedVector<T>(_size);
        MathOperations::subtract<T, ExecutionTraitSIMD<>>(_size, a.data(), b.data(), resSIMD.data());
        TestUtility::compareVectors(resSIMD.data(), expected.data(), _size, "SIMD");

        auto resScaledClassic = createAlignedVector<T>(_size);
        MathOperations::subtract<T, ExecutionTraitClassic<>>(_size, a.data(), b.data(), resScaledClassic.data(), scalarA, scalarB);
        TestUtility::compareVectors(resScaledClassic.data(), expectedScaled.data(), _size, "Classic Scaled");

        auto resScaledMeta = createAlignedVector<T>(_size);
        MathOperations::subtract<T, ExecutionTraitUnrolled<>>(_size, a.data(), b.data(), resScaledMeta.data(), scalarA, scalarB);
        TestUtility::compareVectors(resScaledMeta.data(), expectedScaled.data(), _size, "Meta Scaled");

        auto resScaledSIMD1 = createAlignedVector<T>(_size);
        MathOperations::subtract<T, ExecutionTraitSIMD<T_SIMD::AVX2>>(_size, a.data(), b.data(), resScaledSIMD1.data(), scalarA, scalarB);
        TestUtility::compareVectors<T>(resScaledSIMD1.data(), expectedScaled.data(), _size, "SIMD AVX2 Scaled", _tolerance);

        auto resScaledSIMD2 = createAlignedVector<T>(_size);
        MathOperations::subtract<T, ExecutionTraitSIMD<T_SIMD::AVX512>>(_size, a.data(), b.data(), resScaledSIMD2.data(), scalarA, scalarB);
        TestUtility::compareVectors<T>(resScaledSIMD2.data(), expectedScaled.data(), _size, "SIMD AVX512 Scaled", _tolerance);

        ThreadTraitSTDThread ThreadTrait = ThreadTraitSTDThread();

        auto resClassicThreaded = createAlignedVector<T>(_size);
        MathOperations::subtract<T, ThreadTraitSTDThread, ExecutionTraitClassic<>>(ThreadTrait, _size, a.data(), b.data(), resClassicThreaded.data());
        TestUtility::compareVectors(resClassicThreaded.data(), expected.data(), _size, "Classic Threaded");

        auto resMetaThreaded = createAlignedVector<T>(_size);
        MathOperations::subtract<T, ThreadTraitSTDThread, ExecutionTraitUnrolled<>>(ThreadTrait, _size, a.data(), b.data(), resMetaThreaded.data());
        TestUtility::compareVectors(resMetaThreaded.data(), expected.data(), _size, "Meta Threaded");

        auto resSIMDThreaded = createAlignedVector<T>(_size);
        MathOperations::subtract<T, ThreadTraitSTDThread, ExecutionTraitSIMD<>>(ThreadTrait, _size, a.data(), b.data(), resSIMDThreaded.data());
        TestUtility::compareVectors(resSIMDThreaded.data(), expected.data(), _size, "SIMD Threaded");

        auto resScaledClassicThreaded = createAlignedVector<T>(_size);
        MathOperations::subtract<T, ThreadTraitSTDThread, ExecutionTraitClassic<>>(ThreadTrait, _size, a.data(), b.data(), resScaledClassicThreaded.data(), scalarA, scalarB);
        TestUtility::compareVectors(resScaledClassicThreaded.data(), expectedScaled.data(), _size, "Classic Scaled Threaded");

        auto resScaledMetaThreaded = createAlignedVector<T>(_size);
        MathOperations::subtract<T, ThreadTraitSTDThread, ExecutionTraitUnrolled<>>(ThreadTrait, _size, a.data(), b.data(), resScaledMetaThreaded.data(), scalarA, scalarB);
        TestUtility::compareVectors(resScaledMetaThreaded.data(), expectedScaled.data(), _size, "Meta Scaled Threaded");

        auto resScaledSIMD1Threaded = createAlignedVector<T>(_size);
        MathOperations::subtract<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2>>(ThreadTrait, _size, a.data(), b.data(), resScaledSIMD1Threaded.data(), scalarA, scalarB);
        TestUtility::compareVectors<T>(resScaledSIMD1Threaded.data(), expectedScaled.data(), _size, "SIMD AVX2 Scaled Threaded", _tolerance);

        auto resScaledSIMD2Threaded = createAlignedVector<T>(_size);
        MathOperations::subtract<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512>>(ThreadTrait, _size, a.data(), b.data(), resScaledSIMD2Threaded.data(), scalarA, scalarB);
        TestUtility::compareVectors<T>(resScaledSIMD2Threaded.data(), expectedScaled.data(), _size, "SIMD AVX512 Scaled Threaded", _tolerance);


    }

    template<typename T>
    void _testMultiply() {
        printSubtitle("Multiplication " + typeToString<T>(), T_Color::BARBIE_PINK);
        auto a = createAlignedVector<T>(_size);
        auto b = createAlignedVector<T>(_size);
        Random::uniform<T>(_size, a.data(), 0, 100);
        Random::uniform<T>(_size, b.data(), 0, 100);
        auto scalarA = Random::uniform<T>(0, 100);
        auto scalarB = Random::uniform<T>(0, 100);
        
        auto expected = createAlignedVector<T>(_size);
        auto expectedScaled = createAlignedVector<T>(_size);
        for (size_t i = 0; i < _size; i++) {
            expected[i] = a[i] * b[i];
            expectedScaled[i] = (a[i] * scalarA) * (b[i] * scalarB);
        }
        
        auto resClassic = createAlignedVector<T>(_size);
        MathOperations::multiply<T, ExecutionTraitClassic<>>(_size, a.data(), b.data(), resClassic.data());
        TestUtility::compareVectors(resClassic.data(), expected.data(), _size, "Classic");

        auto resMeta = createAlignedVector<T>(_size);
        MathOperations::multiply<T, ExecutionTraitUnrolled<>>(_size, a.data(), b.data(), resMeta.data());
        TestUtility::compareVectors(resMeta.data(), expected.data(), _size, "Meta");
        
        auto resSIMD = createAlignedVector<T>(_size);
        MathOperations::multiply<T, ExecutionTraitSIMD<>>(_size, a.data(), b.data(), resSIMD.data());
        TestUtility::compareVectors(resSIMD.data(), expected.data(), _size, "SIMD");

        auto resScaledClassic = createAlignedVector<T>(_size);
        MathOperations::multiply<T, ExecutionTraitClassic<>>(_size, a.data(), b.data(), resScaledClassic.data(), scalarA, scalarB);
        TestUtility::compareVectors(resScaledClassic.data(), expectedScaled.data(), _size, "Classic Scaled");

        auto resScaledMeta = createAlignedVector<T>(_size);
        MathOperations::multiply<T, ExecutionTraitUnrolled<>>(_size, a.data(), b.data(), resScaledMeta.data(), scalarA, scalarB);
        TestUtility::compareVectors(resScaledMeta.data(), expectedScaled.data(), _size, "Meta Scaled");

        auto resScaledSIMD1 = createAlignedVector<T>(_size);
        MathOperations::multiply<T, ExecutionTraitSIMD<T_SIMD::AVX2>>(_size, a.data(), b.data(), resScaledSIMD1.data(), scalarA, scalarB);
        TestUtility::compareVectors<T>(resScaledSIMD1.data(), expectedScaled.data(), _size, "SIMD AVX2 Scaled", _tolerance);

        auto resScaledSIMD2 = createAlignedVector<T>(_size);
        MathOperations::multiply<T, ExecutionTraitSIMD<T_SIMD::AVX512>>(_size, a.data(), b.data(), resScaledSIMD2.data(), scalarA, scalarB);
        TestUtility::compareVectors<T>(resScaledSIMD2.data(), expectedScaled.data(), _size, "SIMD AVX512 Scaled", _tolerance);

        ThreadTraitSTDThread ThreadTrait = ThreadTraitSTDThread();

        auto resClassicThreaded = createAlignedVector<T>(_size);
        MathOperations::multiply<T, ThreadTraitSTDThread, ExecutionTraitClassic<>>(ThreadTrait, _size, a.data(), b.data(), resClassicThreaded.data());
        TestUtility::compareVectors(resClassicThreaded.data(), expected.data(), _size, "Classic Threaded");

        auto resMetaThreaded = createAlignedVector<T>(_size);
        MathOperations::multiply<T, ThreadTraitSTDThread, ExecutionTraitUnrolled<>>(ThreadTrait, _size, a.data(), b.data(), resMetaThreaded.data());
        TestUtility::compareVectors(resMetaThreaded.data(), expected.data(), _size, "Meta Threaded");

        // auto resSIMDThreaded = createAlignedVector<T>(_size);
        // MathOperations::multiply<T, ThreadTraitSTDThread, ExecutionTraitSIMD<>>(ThreadTrait, _size, a.data(), b.data(), resSIMDThreaded.data());
        // TestUtility::compareVectors(resSIMDThreaded.data(), expected.data(), _size, "SIMD Threaded");

        auto resScaledClassicThreaded = createAlignedVector<T>(_size);
        MathOperations::multiply<T, ThreadTraitSTDThread, ExecutionTraitClassic<>>(ThreadTrait, _size, a.data(), b.data(), resScaledClassicThreaded.data(), scalarA, scalarB);
        TestUtility::compareVectors(resScaledClassicThreaded.data(), expectedScaled.data(), _size, "Classic Scaled Threaded");

        auto resScaledMetaThreaded = createAlignedVector<T>(_size);
        MathOperations::multiply<T, ThreadTraitSTDThread, ExecutionTraitUnrolled<>>(ThreadTrait, _size, a.data(), b.data(), resScaledMetaThreaded.data(), scalarA, scalarB);
        TestUtility::compareVectors(resScaledMetaThreaded.data(), expectedScaled.data(), _size, "Meta Scaled Threaded");

        auto resScaledSIMD1Threaded = createAlignedVector<T>(_size);
        MathOperations::multiply<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2>>(ThreadTrait, _size, a.data(), b.data(), resScaledSIMD1Threaded.data(), scalarA, scalarB);
        TestUtility::compareVectors<T>(resScaledSIMD1Threaded.data(), expectedScaled.data(), _size, "SIMD AVX2 Scaled Threaded", _tolerance);

        auto resScaledSIMD2Threaded = createAlignedVector<T>(_size);
        MathOperations::multiply<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512>>(ThreadTrait, _size, a.data(), b.data(), resScaledSIMD2Threaded.data(), scalarA, scalarB);
        TestUtility::compareVectors<T>(resScaledSIMD2Threaded.data(), expectedScaled.data(), _size, "SIMD AVX512 Scaled Threaded", _tolerance);
    }

    template<typename T>
    void _testSum() {
        printSubtitle("Sum " + typeToString<T>(), T_Color::BARBIE_PINK);
        auto typeName = typeToString<T>();
        auto unit = Stalker::Core::TimeUnit::nanoseconds;

        auto a = createAlignedVector<T>(_size);
        Random::uniform<T>(_size, a.data(), 0, 10);

        T expected = 0;

        for (size_t i = 0; i < _size; i++) {
            expected += a[i];
        }

        {
            T resClassic = MathOperations::sum<T, ExecutionTraitClassic<>>(_size, a.data());
            TestUtility::compareValues(resClassic, expected, "Classic Sum", _tolerance, true);
        }

        {
            T resMeta = MathOperations::sum<T, ExecutionTraitUnrolled<16>>(_size, a.data());
            TestUtility::compareValues(resMeta, expected, "Meta Sum", _tolerance, true);
        }

        {
            T resSIMD1 = MathOperations::sum<T, ExecutionTraitSIMD<T_SIMD::AVX2>>(_size, a.data());
            TestUtility::compareValues(resSIMD1, expected, "SIMD AVX2 Sum", _tolerance, true);
        }

        {
            T resSIMD2 = MathOperations::sum<T, ExecutionTraitSIMD<T_SIMD::AVX512>>(_size, a.data());
            TestUtility::compareValues(resSIMD2, expected, "SIMD AVX512 Sum", _tolerance, true);
        }

        ThreadTraitSTDThread ThreadTrait = ThreadTraitSTDThread();
        auto nThreads = ThreadTrait.getNumThreads();
        auto nThreadsStr = std::to_string(nThreads);

        {
            // T resClassicThreaded = MathOperations::sum<T, ThreadTraitSTDThread, ExecutionTraitClassic<>>(ThreadTrait, _size, a.data());
            // TestUtility::compareValues(resClassicThreaded, expected, "Classic Threaded Sum", _tolerance);
        }

        {
            // T resMetaThreaded = MathOperations::sum<T, ThreadTraitSTDThread, ExecutionTraitUnrolled<16>>(ThreadTrait, _size, a.data());
            // TestUtility::compareValues(resMetaThreaded, expected, "Meta Threaded Sum", _tolerance);
        }

        {
            // T resSIMD1Threaded = MathOperations::sum<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2>>(ThreadTrait, _size, a.data());
            // TestUtility::compareValues(resSIMD1Threaded, expected, "SIMD AVX2 Threaded Sum", _tolerance);
        }

        {
            // T resSIMD2Threaded = MathOperations::sum<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512>>(ThreadTrait, _size, a.data());
            // TestUtility::compareValues(resSIMD2Threaded, expected, "SIMD AVX512 Threaded Sum", _tolerance);
        }
        
    }

    template<typename T>
    void _testScale() {
        printSubtitle("Scale " + typeToString<T>(), T_Color::BARBIE_PINK);

        // Data
        auto input = createAlignedVector<T>(_size);
        Random::uniform<T>(_size, input.data(), 0, 10);

        // Scalar with safe range for integral types
        T scalar = static_cast<T>(3.14);

        // Expected
        auto expected = createAlignedVector<T>(_size);
        for (size_t i = 0; i < _size; ++i) expected[i] = static_cast<T>(input[i] * scalar);

        // Non-threaded variants (in-place)
        {
            auto a = input;
            MathOperations::scale<T, ExecutionTraitClassic<>>(_size, a.data(), scalar);
            TestUtility::compareVectors<T>(a.data(), expected.data(), _size, "Classic", _tolerance);
        }
        {
            auto a = input;
            MathOperations::scale<T, ExecutionTraitUnrolled<16>>(_size, a.data(), scalar);
            TestUtility::compareVectors<T>(a.data(), expected.data(), _size, "Meta", _tolerance);
        }
        {
            auto a = input;
            MathOperations::scale<T, ExecutionTraitSIMD<T_SIMD::AVX2>>(_size, a.data(), scalar);
            TestUtility::compareVectors<T>(a.data(), expected.data(), _size, "SIMD AVX2", _tolerance);
        }
        {
            auto a = input;
            MathOperations::scale<T, ExecutionTraitSIMD<T_SIMD::AVX512>>(_size, a.data(), scalar);
            TestUtility::compareVectors<T>(a.data(), expected.data(), _size, "SIMD AVX512", _tolerance);
        }

        // Threaded variants (out-of-place)
        ThreadTraitSTDThread ThreadTrait = ThreadTraitSTDThread();

        {
            auto a = input;
            MathOperations::scale<T, ThreadTraitSTDThread, ExecutionTraitClassic<>>(ThreadTrait, _size, a.data(), scalar);
            TestUtility::compareVectors<T>(a.data(), expected.data(), _size, "Classic Threaded", _tolerance);
        }
        {
            auto a = input;
            MathOperations::scale<T, ThreadTraitSTDThread, ExecutionTraitUnrolled<16>>(ThreadTrait, _size, a.data(), scalar);
            TestUtility::compareVectors<T>(a.data(), expected.data(), _size, "Meta Threaded", _tolerance);
        }
        {
            auto a = input;
            MathOperations::scale<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2>>(ThreadTrait, _size, a.data(), scalar);
            TestUtility::compareVectors<T>(a.data(), expected.data(), _size, "SIMD AVX2 Threaded", _tolerance);
        }
        {
            auto a = input;
            MathOperations::scale<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512>>(ThreadTrait, _size, a.data(), scalar);
            TestUtility::compareVectors<T>(a.data(), expected.data(), _size, "SIMD AVX512 Threaded", _tolerance);
        }
    }

    template<typename T>
    void _testAddConstant() {
        printSubtitle("Add Constant " + typeToString<T>(), T_Color::BARBIE_PINK);

        // Data
        auto input = createAlignedVector<T>(_size);
        Random::uniform<T>(_size, input.data(), 0, 10);

        // Scalar with safe range for integral types
        T scalar = static_cast<T>(3.14);

        // Expected
        auto expected = createAlignedVector<T>(_size);
        for (size_t i = 0; i < _size; ++i) expected[i] = static_cast<T>(input[i] + scalar);

        // Non-threaded variants (in-place)
        {
            auto a = input;
            MathOperations::addConstant<T, ExecutionTraitClassic<>>(_size, a.data(), scalar);
            TestUtility::compareVectors<T>(a.data(), expected.data(), _size, "Classic", _tolerance);
        }
        {
            auto a = input;
            MathOperations::addConstant<T, ExecutionTraitUnrolled<16>>(_size, a.data(), scalar);
            TestUtility::compareVectors<T>(a.data(), expected.data(), _size, "Meta", _tolerance);
        }
        {
            auto a = input;
            MathOperations::addConstant<T, ExecutionTraitSIMD<T_SIMD::AVX2>>(_size, a.data(), scalar);
            TestUtility::compareVectors<T>(a.data(), expected.data(), _size, "SIMD AVX2", _tolerance);
        }
        {
            auto a = input;
            MathOperations::addConstant<T, ExecutionTraitSIMD<T_SIMD::AVX512>>(_size, a.data(), scalar);
            TestUtility::compareVectors<T>(a.data(), expected.data(), _size, "SIMD AVX512", _tolerance);
        }

        // Threaded variants (out-of-place)
        ThreadTraitSTDThread ThreadTrait = ThreadTraitSTDThread();

        {
            auto a = input;
            MathOperations::addConstant<T, ThreadTraitSTDThread, ExecutionTraitClassic<>>(ThreadTrait, _size, a.data(), scalar);
            TestUtility::compareVectors<T>(a.data(), expected.data(), _size, "Classic Threaded", _tolerance);
        }
        {
            auto a = input;
            MathOperations::addConstant<T, ThreadTraitSTDThread, ExecutionTraitUnrolled<16>>(ThreadTrait, _size, a.data(), scalar);
            TestUtility::compareVectors<T>(a.data(), expected.data(), _size, "Meta Threaded", _tolerance);
        }
        {
            auto a = input;
            MathOperations::addConstant<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2>>(ThreadTrait, _size, a.data(), scalar);
            TestUtility::compareVectors<T>(a.data(), expected.data(), _size, "SIMD AVX2 Threaded", _tolerance);
        }
        {
            auto a = input;
            MathOperations::addConstant<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512>>(ThreadTrait, _size, a.data(), scalar);
            TestUtility::compareVectors<T>(a.data(), expected.data(), _size, "SIMD AVX512 Threaded", _tolerance);
        }
    }

    template<typename T>
    void _testDotProduct() {
        printSubtitle("Dot Product " + typeToString<T>(), T_Color::BARBIE_PINK);

        // Data
        auto a = createAlignedVector<T>(_size);
        auto b = createAlignedVector<T>(_size);
        Random::uniform<T>(_size, a.data(), 0, 10);
        Random::uniform<T>(_size, b.data(), 0, 10);

        // Expected
        T expected = 0;
        for (size_t i = 0; i < _size; ++i) expected += a[i] * b[i];

        // Non-threaded variants (in-place)
        auto tolerance = 1e-4;
        {
            auto res = MathOperations::dot<T, ExecutionTraitClassic<>>(_size, a.data(), b.data());
            TestUtility::compareValues<T>(res, expected, "Classic", tolerance, true);
        }
        {
            auto res = MathOperations::dot<T, ExecutionTraitUnrolled<1>>(_size, a.data(), b.data());
            TestUtility::compareValues<T>(res, expected, "Meta", tolerance, true);
        }
        {
            auto res = MathOperations::dot<T, ExecutionTraitSIMD<T_SIMD::AVX2>>(_size, a.data(), b.data());
            TestUtility::compareValues<T>(res, expected, "SIMD AVX2", tolerance, true);
        }
        {
            auto res = MathOperations::dot<T, ExecutionTraitSIMD<T_SIMD::AVX512>>(_size, a.data(), b.data());
            TestUtility::compareValues<T>(res, expected, "SIMD AVX512", _tolerance, true);
        }
    }
};


} // namespace STLKR_Tests