/*
 * Copyright 2024-2026 Papas Christoforos Orestis
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "STLKR_TestBase.h"
#include <Stalker/Mathematics/Vector/VectorMath.h>

namespace STLKR_Tests {

    using namespace Stalker::Mathematics;
    using namespace Stalker::Memory;

    class MathOperationsTests : public STLKR_TestBase {
    public:

    explicit MathOperationsTests(size_t size = 10000, double tolerance = 1e-2)  
                : STLKR_TestBase/*  */(" VectorMath"), _size(size), _tolerance(tolerance) {
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


        // auto logs = _logs.

        auto a = createAlignedVector<T>(_size);
        auto b = createAlignedVector<T>(_size);
        Random::uniform<T>(_size, a.data(), 0, 10);
        Random::uniform<T>(_size, b.data(), 0, 10);
        auto scalarA = Random::uniform<T>(0, 10);
        auto scalarB = Random::uniform<T>(0, 10);

        auto expected = createAlignedVector<T>(_size);
        auto expectedScaled = createAlignedVector<T>(_size);
        for (size_t i = 0; i < _size; i++) {
            expected[i] = a[i] + b[i];
            expectedScaled[i] = (a[i] * scalarA) + (b[i] * scalarB);
        }
        
        {
            auto resClassic = createAlignedVector<T>(_size);
             VectorMath::add<T, ExecutionTraitScalar<>>(_size, a.data(), b.data(), resClassic.data());
            TestUtility::compareVectors(resClassic.data(), expected.data(), _size, "Scalar");
        }

        {
            auto resMeta = createAlignedVector<T>(_size);
             VectorMath::add<T, ExecutionTraitUnrolled<>>(_size, a.data(), b.data(), resMeta.data());
            TestUtility::compareVectors(resMeta.data(), expected.data(), _size, "Meta");
        }
        #if defined(STALKER_SIMD_AVX2_OK)
        {
            auto resSIMDAligned = createAlignedVector<T>(_size);
             VectorMath::add<T, ExecutionTraitSIMD<T_SIMD::AVX2, true, DefaultSIMDStore(), 1>>(_size, a.data(), b.data(), resSIMDAligned.data());
            TestUtility::compareVectors<T>(resSIMDAligned.data(), expected.data(), _size, "SIMD AVX2 Aligned", _tolerance);

            auto resSIMDUnaligned = createAlignedVector<T>(_size);
             VectorMath::add<T, ExecutionTraitSIMD<T_SIMD::AVX2, false, DefaultSIMDStore(), 1>>(_size, a.data(), b.data(), resSIMDUnaligned.data());
            TestUtility::compareVectors<T>(resSIMDUnaligned.data(), expected.data(), _size, "SIMD AVX2 Unaligned", _tolerance);
        }
        #endif
        #if defined(STALKER_SIMD_AVX512_OK)
        {
            auto resSIMDAligned = createAlignedVector<T>(_size);
             VectorMath::add<T, ExecutionTraitSIMD<T_SIMD::AVX512, true, DefaultSIMDStore(), 1>>(_size, a.data(), b.data(), resSIMDAligned.data());
            TestUtility::compareVectors<T>(resSIMDAligned.data(), expected.data(), _size, "SIMD AVX512 Aligned", _tolerance);

            auto resSIMDUnaligned = createAlignedVector<T>(_size);
             VectorMath::add<T, ExecutionTraitSIMD<T_SIMD::AVX512, false, DefaultSIMDStore(), 1>>(_size, a.data(), b.data(), resSIMDUnaligned.data());
            TestUtility::compareVectors<T>(resSIMDUnaligned.data(), expected.data(), _size, "SIMD AVX512 Unaligned", _tolerance);
        }
        #endif

        {
            auto resScaledClassic = createAlignedVector<T>(_size);
             VectorMath::add<T, ExecutionTraitScalar<>>(_size, a.data(), b.data(), resScaledClassic.data(), scalarA, scalarB);
            TestUtility::compareVectors(resScaledClassic.data(), expectedScaled.data(), _size, "Scalar Scaled");
        }

        {
            auto resScaledMeta = createAlignedVector<T>(_size);
             VectorMath::add<T, ExecutionTraitUnrolled<>>(_size, a.data(), b.data(), resScaledMeta.data(), scalarA, scalarB);
            TestUtility::compareVectors(resScaledMeta.data(), expectedScaled.data(), _size, "Meta Scaled");
        }

        #if defined(STALKER_SIMD_AVX2_OK)
        {
            auto resScaledSIMDAligned = createAlignedVector<T>(_size);
             VectorMath::add<T, ExecutionTraitSIMD<T_SIMD::AVX2, true, DefaultSIMDStore(), 1>>(_size, a.data(), b.data(), resScaledSIMDAligned.data(), scalarA, scalarB);
            TestUtility::compareVectors<T>(resScaledSIMDAligned.data(), expectedScaled.data(), _size, "SIMD AVX2 Scaled Aligned", _tolerance);

            auto resScaledSIMDUnaligned = createAlignedVector<T>(_size);
             VectorMath::add<T, ExecutionTraitSIMD<T_SIMD::AVX2, false, DefaultSIMDStore(), 1>>(_size, a.data(), b.data(), resScaledSIMDUnaligned.data(), scalarA, scalarB);
            TestUtility::compareVectors<T>(resScaledSIMDUnaligned.data(), expectedScaled.data(), _size, "SIMD AVX2 Scaled Unaligned", _tolerance);
        }
        #endif
        #if defined(STALKER_SIMD_AVX512_OK)
        {
            auto resScaledSIMDAligned = createAlignedVector<T>(_size);
             VectorMath::add<T, ExecutionTraitSIMD<T_SIMD::AVX512, true, DefaultSIMDStore(), 2>>(_size, a.data(), b.data(), resScaledSIMDAligned.data(), scalarA, scalarB);
            TestUtility::compareVectors<T>(resScaledSIMDAligned.data(), expectedScaled.data(), _size, "SIMD AVX512 Scaled Aligned", _tolerance);

            auto resScaledSIMDUnaligned = createAlignedVector<T>(_size);
             VectorMath::add<T, ExecutionTraitSIMD<T_SIMD::AVX512, false, DefaultSIMDStore(), 2>>(_size, a.data(), b.data(), resScaledSIMDUnaligned.data(), scalarA, scalarB);
            TestUtility::compareVectors<T>(resScaledSIMDUnaligned.data(), expectedScaled.data(), _size, "SIMD AVX512 Scaled Unaligned", _tolerance);
        }
        #endif

        #if defined(STALKER_THREADING_ENABLE) && STALKER_THREADING_ENABLE != 0

        ThreadTraitSTDThread ThreadTrait = ThreadTraitSTDThread();
        auto nThreads = ThreadTrait.getNumThreads();
        auto nThreadsStr = std::to_string(nThreads);

        {
            auto resClassicThreaded = createAlignedVector<T>(_size);
             VectorMath::add<T, ThreadTraitSTDThread, ExecutionTraitScalar<>>(ThreadTrait, _size, a.data(), b.data(), resClassicThreaded.data());
            TestUtility::compareVectors(resClassicThreaded.data(), expected.data(), _size, "Scalar Threaded");
        }

        {
            auto resMetaThreaded = createAlignedVector<T>(_size);
             VectorMath::add<T, ThreadTraitSTDThread, ExecutionTraitUnrolled<>>(ThreadTrait, _size, a.data(), b.data(), resMetaThreaded.data());
            TestUtility::compareVectors(resMetaThreaded.data(), expected.data(), _size, "Meta Threaded");
        }

        {
            auto resMetaThreaded = createAlignedVector<T>(_size);
             VectorMath::add<T, ThreadTraitSTDThread, ExecutionTraitUnrolled<>>(ThreadTrait, _size, a.data(), b.data(), resMetaThreaded.data());
            TestUtility::compareVectors(resMetaThreaded.data(), expected.data(), _size, "Meta Threaded");
        }

        #if defined(STALKER_SIMD_AVX2_OK)
        {
            auto resSIMD1Threaded = createAlignedVector<T>(_size);
             VectorMath::add<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2, true>>(ThreadTrait, _size, a.data(), b.data(), resSIMD1Threaded.data());
            TestUtility::compareVectors<T>(resSIMD1Threaded.data(), expected.data(), _size, "SIMD AVX2 Threaded Aligned", _tolerance);

            auto resSIMD1ThreadedUnaligned = createAlignedVector<T>(_size);
             VectorMath::add<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2, false>>(ThreadTrait, _size, a.data(), b.data(), resSIMD1ThreadedUnaligned.data());
            TestUtility::compareVectors<T>(resSIMD1ThreadedUnaligned.data(), expected.data(), _size, "SIMD AVX2 Threaded Unaligned", _tolerance);
        }
        #endif
        #if defined(STALKER_SIMD_AVX512_OK)
        {
            auto resSIMD2Threaded = createAlignedVector<T>(_size);
             VectorMath::add<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512, true, DefaultSIMDStore(), 1>>(ThreadTrait, _size, a.data(), b.data(), resSIMD2Threaded.data());
            TestUtility::compareVectors<T>(resSIMD2Threaded.data(), expected.data(), _size, "SIMD AVX512 Threaded Aligned", _tolerance);

            auto resSIMD2ThreadedUnaligned = createAlignedVector<T>(_size);
             VectorMath::add<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512, false, DefaultSIMDStore(), 1>>(ThreadTrait, _size, a.data(), b.data(), resSIMD2ThreadedUnaligned.data());
            TestUtility::compareVectors<T>(resSIMD2ThreadedUnaligned.data(), expected.data(), _size, "SIMD AVX512 Threaded Unaligned", _tolerance);
        }
        #endif
        {
            auto resScaledClassicThreaded = createAlignedVector<T>(_size);
             VectorMath::add<T, ThreadTraitSTDThread, ExecutionTraitScalar<>>(ThreadTrait, _size, a.data(), b.data(), resScaledClassicThreaded.data(), scalarA, scalarB);
            TestUtility::compareVectors(resScaledClassicThreaded.data(), expectedScaled.data(), _size, "Scalar Scaled Threaded");
        }

        {
            auto resScaledMetaThreaded = createAlignedVector<T>(_size);
             VectorMath::add<T, ThreadTraitSTDThread, ExecutionTraitUnrolled<>>(ThreadTrait, _size, a.data(), b.data(), resScaledMetaThreaded.data(), scalarA, scalarB);
            TestUtility::compareVectors(resScaledMetaThreaded.data(), expectedScaled.data(), _size, "Meta Scaled Threaded");
        }

        #if defined(STALKER_SIMD_AVX2_OK)
        {
            auto resScaledSIMD1Threaded = createAlignedVector<T>(_size);
             VectorMath::add<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2, true>>(ThreadTrait, _size, a.data(), b.data(), resScaledSIMD1Threaded.data(), scalarA, scalarB);
            TestUtility::compareVectors<T>(resScaledSIMD1Threaded.data(), expectedScaled.data(), _size, "SIMD AVX2 Scaled Threaded Aligned", _tolerance);

            auto resScaledSIMD1ThreadedUnaligned = createAlignedVector<T>(_size);
             VectorMath::add<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2, false>>(ThreadTrait, _size, a.data(), b.data(), resScaledSIMD1ThreadedUnaligned.data(), scalarA, scalarB);
            TestUtility::compareVectors<T>(resScaledSIMD1ThreadedUnaligned.data(), expectedScaled.data(), _size, "SIMD AVX2 Scaled Threaded Unaligned", _tolerance);
        }
        #endif
        #if defined(STALKER_SIMD_AVX512_OK)
        {
            auto resScaledSIMD2Threaded = createAlignedVector<T>(_size);
             VectorMath::add<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512, true>>(ThreadTrait, _size, a.data(), b.data(), resScaledSIMD2Threaded.data(), scalarA, scalarB);
            TestUtility::compareVectors<T>(resScaledSIMD2Threaded.data(), expectedScaled.data(), _size, "SIMD AVX512 Scaled Threaded Aligned", _tolerance);

            auto resScaledSIMD2ThreadedUnaligned = createAlignedVector<T>(_size);
             VectorMath::add<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512, false>>(ThreadTrait, _size, a.data(), b.data(), resScaledSIMD2ThreadedUnaligned.data(), scalarA, scalarB);
            TestUtility::compareVectors<T>(resScaledSIMD2ThreadedUnaligned.data(), expectedScaled.data(), _size, "SIMD AVX512 Scaled Threaded Unaligned", _tolerance);
        }
        #endif
        #endif
    }

    template<typename T>
    void _testAxpy() {

        printSubtitle("Axpy " + typeToString<T>(), T_Color::BARBIE_PINK);
        auto typeName = typeToString<T>();

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
             VectorMath::axpy<T, ExecutionTraitScalar<>>(_size, a.data(), b.data(), resClassic.data(), scalarA);
            TestUtility::compareVectors(resClassic.data(), expected.data(), _size, "Scalar");
        }

        {
            auto resMeta = createAlignedVector<T>(_size);
             VectorMath::axpy<T, ExecutionTraitUnrolled<>>(_size, a.data(), b.data(), resMeta.data(), scalarA);
            TestUtility::compareVectors(resMeta.data(), expected.data(), _size, "Meta");
        }
        
        #if defined(STALKER_SIMD_AVX2_OK)
        {
            auto resSIMDAligned = createAlignedVector<T>(_size);
             VectorMath::axpy<T, ExecutionTraitSIMD<T_SIMD::AVX2, true, DefaultSIMDStore(), 1>>(_size, a.data(), b.data(), resSIMDAligned.data(), scalarA);
            TestUtility::compareVectors<T>(resSIMDAligned.data(), expected.data(), _size, "SIMD AVX2 Aligned", _tolerance);

            auto resSIMDUnaligned = createAlignedVector<T>(_size);
             VectorMath::axpy<T, ExecutionTraitSIMD<T_SIMD::AVX2, false, DefaultSIMDStore(), 1>>(_size, a.data(), b.data(), resSIMDUnaligned.data(), scalarA);
            TestUtility::compareVectors<T>(resSIMDUnaligned.data(), expected.data(), _size, "SIMD AVX2 Unaligned", _tolerance);
        }
        #endif

        #if defined(STALKER_SIMD_AVX512_OK)
        {
            auto resSIMDAligned = createAlignedVector<T>(_size);
             VectorMath::axpy<T, ExecutionTraitSIMD<T_SIMD::AVX512, true, DefaultSIMDStore(), 1>>(_size, a.data(), b.data(), resSIMDAligned.data(), scalarA);
            TestUtility::compareVectors<T>(resSIMDAligned.data(), expected.data(), _size, "SIMD AVX512 Aligned", _tolerance);

            auto resSIMDUnaligned = createAlignedVector<T>(_size);
             VectorMath::axpy<T, ExecutionTraitSIMD<T_SIMD::AVX512, false, DefaultSIMDStore(), 1>>(_size, a.data(), b.data(), resSIMDUnaligned.data(), scalarA);
            TestUtility::compareVectors<T>(resSIMDUnaligned.data(), expected.data(), _size, "SIMD AVX512 Unaligned", _tolerance);
        }
        #endif

        #if defined(STALKER_THREADING_ENABLE) && STALKER_THREADING_ENABLE != 0

        ThreadTraitSTDThread ThreadTrait = ThreadTraitSTDThread();
        auto nThreads = ThreadTrait.getNumThreads();
        auto nThreadsStr = std::to_string(nThreads);

        {
            auto resClassic = createAlignedVector<T>(_size);
             VectorMath::axpy<T, ThreadTraitSTDThread, ExecutionTraitScalar<>>(ThreadTrait, _size, a.data(), b.data(), resClassic.data(), scalarA);
            TestUtility::compareVectors(resClassic.data(), expected.data(), _size, "Scalar Threaded (" + nThreadsStr + " threads)");
        }

        {
            auto resMeta = createAlignedVector<T>(_size);
             VectorMath::axpy<T, ThreadTraitSTDThread, ExecutionTraitUnrolled<>>(ThreadTrait, _size, a.data(), b.data(), resMeta.data(), scalarA);
            TestUtility::compareVectors(resMeta.data(), expected.data(), _size, "Meta Threaded (" + nThreadsStr + " threads)");
        }
        
        #if defined(STALKER_SIMD_AVX2_OK)
        {
            auto resSIMD1 = createAlignedVector<T>(_size);
             VectorMath::axpy<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2, true, DefaultSIMDStore(), 1>>(ThreadTrait, _size, a.data(), b.data(), resSIMD1.data(), scalarA);
            TestUtility::compareVectors<T>(resSIMD1.data(), expected.data(), _size, "SIMD AVX2 Threaded Aligned (" + nThreadsStr + " threads)", _tolerance);

            auto resSIMD1Unaligned = createAlignedVector<T>(_size);
             VectorMath::axpy<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2, false, DefaultSIMDStore(), 1>>(ThreadTrait, _size, a.data(), b.data(), resSIMD1Unaligned.data(), scalarA);
            TestUtility::compareVectors<T>(resSIMD1Unaligned.data(), expected.data(), _size, "SIMD AVX2 Threaded Unaligned (" + nThreadsStr + " threads)", _tolerance);
        }
        #endif

        #if defined(STALKER_SIMD_AVX512_OK)
        {
            auto resSIMD2 = createAlignedVector<T>(_size);
             VectorMath::axpy<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512, true, DefaultSIMDStore(), 1>>(ThreadTrait, _size, a.data(), b.data(), resSIMD2.data(), scalarA);
            TestUtility::compareVectors<T>(resSIMD2.data(), expected.data(), _size, "SIMD AVX512 Threaded Aligned (" + nThreadsStr + " threads)", _tolerance);

            auto resSIMD2Unaligned = createAlignedVector<T>(_size);
             VectorMath::axpy<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512, false, DefaultSIMDStore(), 1>>(ThreadTrait, _size, a.data(), b.data(), resSIMD2Unaligned.data(), scalarA);
            TestUtility::compareVectors<T>(resSIMD2Unaligned.data(), expected.data(), _size, "SIMD AVX512 Threaded Unaligned (" + nThreadsStr + " threads)", _tolerance);
        }
        #endif

        #endif
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
         VectorMath::subtract<T, ExecutionTraitScalar<>>(_size, a.data(), b.data(), resClassic.data());
        TestUtility::compareVectors(resClassic.data(), expected.data(), _size, "Scalar");

        auto resMeta = createAlignedVector<T>(_size);
         VectorMath::subtract<T, ExecutionTraitUnrolled<>>(_size, a.data(), b.data(), resMeta.data());
        TestUtility::compareVectors(resMeta.data(), expected.data(), _size, "Meta");
        
    #if defined(STALKER_SIMD_AVX2_OK)
    auto resSIMDAVX2Aligned = createAlignedVector<T>(_size);
     VectorMath::subtract<T, ExecutionTraitSIMD<T_SIMD::AVX2, true>>(_size, a.data(), b.data(), resSIMDAVX2Aligned.data());
    TestUtility::compareVectors<T>(resSIMDAVX2Aligned.data(), expected.data(), _size, "SIMD AVX2 Aligned", _tolerance);

    auto resSIMDAVX2Unaligned = createAlignedVector<T>(_size);
     VectorMath::subtract<T, ExecutionTraitSIMD<T_SIMD::AVX2, false>>(_size, a.data(), b.data(), resSIMDAVX2Unaligned.data());
    TestUtility::compareVectors<T>(resSIMDAVX2Unaligned.data(), expected.data(), _size, "SIMD AVX2 Unaligned", _tolerance);
    #endif

    #if defined(STALKER_SIMD_AVX512_OK)
    auto resSIMDAVX512Aligned = createAlignedVector<T>(_size);
     VectorMath::subtract<T, ExecutionTraitSIMD<T_SIMD::AVX512, true>>(_size, a.data(), b.data(), resSIMDAVX512Aligned.data());
    TestUtility::compareVectors<T>(resSIMDAVX512Aligned.data(), expected.data(), _size, "SIMD AVX512 Aligned", _tolerance);

    auto resSIMDAVX512Unaligned = createAlignedVector<T>(_size);
     VectorMath::subtract<T, ExecutionTraitSIMD<T_SIMD::AVX512, false>>(_size, a.data(), b.data(), resSIMDAVX512Unaligned.data());
    TestUtility::compareVectors<T>(resSIMDAVX512Unaligned.data(), expected.data(), _size, "SIMD AVX512 Unaligned", _tolerance);
    #endif

        auto resScaledClassic = createAlignedVector<T>(_size);
         VectorMath::subtract<T, ExecutionTraitScalar<>>(_size, a.data(), b.data(), resScaledClassic.data(), scalarA, scalarB);
        TestUtility::compareVectors(resScaledClassic.data(), expectedScaled.data(), _size, "Scalar Scaled");

        auto resScaledMeta = createAlignedVector<T>(_size);
         VectorMath::subtract<T, ExecutionTraitUnrolled<>>(_size, a.data(), b.data(), resScaledMeta.data(), scalarA, scalarB);
        TestUtility::compareVectors(resScaledMeta.data(), expectedScaled.data(), _size, "Meta Scaled");

    #if defined(STALKER_SIMD_AVX2_OK)
    auto resScaledSIMD1Aligned = createAlignedVector<T>(_size);
     VectorMath::subtract<T, ExecutionTraitSIMD<T_SIMD::AVX2, true>>(_size, a.data(), b.data(), resScaledSIMD1Aligned.data(), scalarA, scalarB);
    TestUtility::compareVectors<T>(resScaledSIMD1Aligned.data(), expectedScaled.data(), _size, "SIMD AVX2 Scaled Aligned", _tolerance);

    auto resScaledSIMD1Unaligned = createAlignedVector<T>(_size);
     VectorMath::subtract<T, ExecutionTraitSIMD<T_SIMD::AVX2, false>>(_size, a.data(), b.data(), resScaledSIMD1Unaligned.data(), scalarA, scalarB);
    TestUtility::compareVectors<T>(resScaledSIMD1Unaligned.data(), expectedScaled.data(), _size, "SIMD AVX2 Scaled Unaligned", _tolerance);
    #endif

    #if defined(STALKER_SIMD_AVX512_OK)
    auto resScaledSIMD2Aligned = createAlignedVector<T>(_size);
     VectorMath::subtract<T, ExecutionTraitSIMD<T_SIMD::AVX512, true>>(_size, a.data(), b.data(), resScaledSIMD2Aligned.data(), scalarA, scalarB);
    TestUtility::compareVectors<T>(resScaledSIMD2Aligned.data(), expectedScaled.data(), _size, "SIMD AVX512 Scaled Aligned", _tolerance);

    auto resScaledSIMD2Unaligned = createAlignedVector<T>(_size);
     VectorMath::subtract<T, ExecutionTraitSIMD<T_SIMD::AVX512, false>>(_size, a.data(), b.data(), resScaledSIMD2Unaligned.data(), scalarA, scalarB);
    TestUtility::compareVectors<T>(resScaledSIMD2Unaligned.data(), expectedScaled.data(), _size, "SIMD AVX512 Scaled Unaligned", _tolerance);
    #endif

        #if defined(STALKER_THREADING_ENABLE) && STALKER_THREADING_ENABLE != 0

        ThreadTraitSTDThread ThreadTrait = ThreadTraitSTDThread();

        auto resClassicThreaded = createAlignedVector<T>(_size);
         VectorMath::subtract<T, ThreadTraitSTDThread, ExecutionTraitScalar<>>(ThreadTrait, _size, a.data(), b.data(), resClassicThreaded.data());
        TestUtility::compareVectors(resClassicThreaded.data(), expected.data(), _size, "Scalar Threaded");

        auto resMetaThreaded = createAlignedVector<T>(_size);
         VectorMath::subtract<T, ThreadTraitSTDThread, ExecutionTraitUnrolled<>>(ThreadTrait, _size, a.data(), b.data(), resMetaThreaded.data());
        TestUtility::compareVectors(resMetaThreaded.data(), expected.data(), _size, "Meta Threaded");

    #if defined(STALKER_SIMD_AVX2_OK)
    auto resSIMDThreadedAVX2Aligned = createAlignedVector<T>(_size);
     VectorMath::subtract<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2, true>>(ThreadTrait, _size, a.data(), b.data(), resSIMDThreadedAVX2Aligned.data());
    TestUtility::compareVectors<T>(resSIMDThreadedAVX2Aligned.data(), expected.data(), _size, "SIMD AVX2 Threaded Aligned", _tolerance);

    auto resSIMDThreadedAVX2Unaligned = createAlignedVector<T>(_size);
     VectorMath::subtract<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2, false>>(ThreadTrait, _size, a.data(), b.data(), resSIMDThreadedAVX2Unaligned.data());
    TestUtility::compareVectors<T>(resSIMDThreadedAVX2Unaligned.data(), expected.data(), _size, "SIMD AVX2 Threaded Unaligned", _tolerance);
    #endif

    #if defined(STALKER_SIMD_AVX512_OK)
    auto resSIMDThreadedAVX512Aligned = createAlignedVector<T>(_size);
     VectorMath::subtract<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512, true>>(ThreadTrait, _size, a.data(), b.data(), resSIMDThreadedAVX512Aligned.data());
    TestUtility::compareVectors<T>(resSIMDThreadedAVX512Aligned.data(), expected.data(), _size, "SIMD AVX512 Threaded Aligned", _tolerance);

    auto resSIMDThreadedAVX512Unaligned = createAlignedVector<T>(_size);
     VectorMath::subtract<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512, false>>(ThreadTrait, _size, a.data(), b.data(), resSIMDThreadedAVX512Unaligned.data());
    TestUtility::compareVectors<T>(resSIMDThreadedAVX512Unaligned.data(), expected.data(), _size, "SIMD AVX512 Threaded Unaligned", _tolerance);
    #endif

        auto resScaledClassicThreaded = createAlignedVector<T>(_size);
         VectorMath::subtract<T, ThreadTraitSTDThread, ExecutionTraitScalar<>>(ThreadTrait, _size, a.data(), b.data(), resScaledClassicThreaded.data(), scalarA, scalarB);
        TestUtility::compareVectors(resScaledClassicThreaded.data(), expectedScaled.data(), _size, "Scalar Scaled Threaded");

        auto resScaledMetaThreaded = createAlignedVector<T>(_size);
         VectorMath::subtract<T, ThreadTraitSTDThread, ExecutionTraitUnrolled<>>(ThreadTrait, _size, a.data(), b.data(), resScaledMetaThreaded.data(), scalarA, scalarB);
        TestUtility::compareVectors(resScaledMetaThreaded.data(), expectedScaled.data(), _size, "Meta Scaled Threaded");

    #if defined(STALKER_SIMD_AVX2_OK)
    auto resScaledSIMD1ThreadedAligned = createAlignedVector<T>(_size);
     VectorMath::subtract<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2, true>>(ThreadTrait, _size, a.data(), b.data(), resScaledSIMD1ThreadedAligned.data(), scalarA, scalarB);
    TestUtility::compareVectors<T>(resScaledSIMD1ThreadedAligned.data(), expectedScaled.data(), _size, "SIMD AVX2 Scaled Threaded Aligned", _tolerance);

    auto resScaledSIMD1ThreadedUnaligned = createAlignedVector<T>(_size);
     VectorMath::subtract<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2, false>>(ThreadTrait, _size, a.data(), b.data(), resScaledSIMD1ThreadedUnaligned.data(), scalarA, scalarB);
    TestUtility::compareVectors<T>(resScaledSIMD1ThreadedUnaligned.data(), expectedScaled.data(), _size, "SIMD AVX2 Scaled Threaded Unaligned", _tolerance);
    #endif

    #if defined(STALKER_SIMD_AVX512_OK)
    auto resScaledSIMD2ThreadedAligned = createAlignedVector<T>(_size);
     VectorMath::subtract<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512, true>>(ThreadTrait, _size, a.data(), b.data(), resScaledSIMD2ThreadedAligned.data(), scalarA, scalarB);
    TestUtility::compareVectors<T>(resScaledSIMD2ThreadedAligned.data(), expectedScaled.data(), _size, "SIMD AVX512 Scaled Threaded Aligned", _tolerance);

    auto resScaledSIMD2ThreadedUnaligned = createAlignedVector<T>(_size);
     VectorMath::subtract<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512, false>>(ThreadTrait, _size, a.data(), b.data(), resScaledSIMD2ThreadedUnaligned.data(), scalarA, scalarB);
    TestUtility::compareVectors<T>(resScaledSIMD2ThreadedUnaligned.data(), expectedScaled.data(), _size, "SIMD AVX512 Scaled Threaded Unaligned", _tolerance);
    #endif
        #endif
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
         VectorMath::multiply<T, ExecutionTraitScalar<>>(_size, a.data(), b.data(), resClassic.data());
        TestUtility::compareVectors(resClassic.data(), expected.data(), _size, "Scalar");

        auto resMeta = createAlignedVector<T>(_size);
         VectorMath::multiply<T, ExecutionTraitUnrolled<>>(_size, a.data(), b.data(), resMeta.data());
        TestUtility::compareVectors(resMeta.data(), expected.data(), _size, "Meta");
        
    #if defined(STALKER_SIMD_AVX2_OK)
    auto resSIMDAVX2Aligned = createAlignedVector<T>(_size);
     VectorMath::multiply<T, ExecutionTraitSIMD<T_SIMD::AVX2, true>>(_size, a.data(), b.data(), resSIMDAVX2Aligned.data());
    TestUtility::compareVectors<T>(resSIMDAVX2Aligned.data(), expected.data(), _size, "SIMD AVX2 Aligned", _tolerance);

    auto resSIMDAVX2Unaligned = createAlignedVector<T>(_size);
     VectorMath::multiply<T, ExecutionTraitSIMD<T_SIMD::AVX2, false>>(_size, a.data(), b.data(), resSIMDAVX2Unaligned.data());
    TestUtility::compareVectors<T>(resSIMDAVX2Unaligned.data(), expected.data(), _size, "SIMD AVX2 Unaligned", _tolerance);
    #endif

    #if defined(STALKER_SIMD_AVX512_OK)
    auto resSIMDAVX512Aligned = createAlignedVector<T>(_size);
     VectorMath::multiply<T, ExecutionTraitSIMD<T_SIMD::AVX512, true>>(_size, a.data(), b.data(), resSIMDAVX512Aligned.data());
    TestUtility::compareVectors<T>(resSIMDAVX512Aligned.data(), expected.data(), _size, "SIMD AVX512 Aligned", _tolerance);

    auto resSIMDAVX512Unaligned = createAlignedVector<T>(_size);
     VectorMath::multiply<T, ExecutionTraitSIMD<T_SIMD::AVX512, false>>(_size, a.data(), b.data(), resSIMDAVX512Unaligned.data());
    TestUtility::compareVectors<T>(resSIMDAVX512Unaligned.data(), expected.data(), _size, "SIMD AVX512 Unaligned", _tolerance);
    #endif

        auto resScaledClassic = createAlignedVector<T>(_size);
         VectorMath::multiply<T, ExecutionTraitScalar<>>(_size, a.data(), b.data(), resScaledClassic.data(), scalarA, scalarB);
        TestUtility::compareVectors(resScaledClassic.data(), expectedScaled.data(), _size, "Scalar Scaled");

        auto resScaledMeta = createAlignedVector<T>(_size);
         VectorMath::multiply<T, ExecutionTraitUnrolled<>>(_size, a.data(), b.data(), resScaledMeta.data(), scalarA, scalarB);
        TestUtility::compareVectors(resScaledMeta.data(), expectedScaled.data(), _size, "Meta Scaled");

    #if defined(STALKER_SIMD_AVX2_OK)
    auto resScaledSIMD1Aligned = createAlignedVector<T>(_size);
     VectorMath::multiply<T, ExecutionTraitSIMD<T_SIMD::AVX2, true>>(_size, a.data(), b.data(), resScaledSIMD1Aligned.data(), scalarA, scalarB);
    TestUtility::compareVectors<T>(resScaledSIMD1Aligned.data(), expectedScaled.data(), _size, "SIMD AVX2 Scaled Aligned", _tolerance);

    auto resScaledSIMD1Unaligned = createAlignedVector<T>(_size);
     VectorMath::multiply<T, ExecutionTraitSIMD<T_SIMD::AVX2, false>>(_size, a.data(), b.data(), resScaledSIMD1Unaligned.data(), scalarA, scalarB);
    TestUtility::compareVectors<T>(resScaledSIMD1Unaligned.data(), expectedScaled.data(), _size, "SIMD AVX2 Scaled Unaligned", _tolerance);
    #endif

    #if defined(STALKER_SIMD_AVX512_OK)
    auto resScaledSIMD2Aligned = createAlignedVector<T>(_size);
     VectorMath::multiply<T, ExecutionTraitSIMD<T_SIMD::AVX512, true>>(_size, a.data(), b.data(), resScaledSIMD2Aligned.data(), scalarA, scalarB);
    TestUtility::compareVectors<T>(resScaledSIMD2Aligned.data(), expectedScaled.data(), _size, "SIMD AVX512 Scaled Aligned", _tolerance);

    auto resScaledSIMD2Unaligned = createAlignedVector<T>(_size);
     VectorMath::multiply<T, ExecutionTraitSIMD<T_SIMD::AVX512, false>>(_size, a.data(), b.data(), resScaledSIMD2Unaligned.data(), scalarA, scalarB);
    TestUtility::compareVectors<T>(resScaledSIMD2Unaligned.data(), expectedScaled.data(), _size, "SIMD AVX512 Scaled Unaligned", _tolerance);
    #endif

        #if defined(STALKER_THREADING_ENABLE) && STALKER_THREADING_ENABLE != 0
        ThreadTraitSTDThread ThreadTrait = ThreadTraitSTDThread();

        auto resClassicThreaded = createAlignedVector<T>(_size);
         VectorMath::multiply<T, ThreadTraitSTDThread, ExecutionTraitScalar<>>(ThreadTrait, _size, a.data(), b.data(), resClassicThreaded.data());
        TestUtility::compareVectors(resClassicThreaded.data(), expected.data(), _size, "Scalar Threaded");

        auto resMetaThreaded = createAlignedVector<T>(_size);
         VectorMath::multiply<T, ThreadTraitSTDThread, ExecutionTraitUnrolled<>>(ThreadTrait, _size, a.data(), b.data(), resMetaThreaded.data());
        TestUtility::compareVectors(resMetaThreaded.data(), expected.data(), _size, "Meta Threaded");

    #if defined(STALKER_SIMD_AVX2_OK)
    auto resSIMDThreadedAVX2Aligned = createAlignedVector<T>(_size);
     VectorMath::multiply<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2, true>>(ThreadTrait, _size, a.data(), b.data(), resSIMDThreadedAVX2Aligned.data());
    TestUtility::compareVectors<T>(resSIMDThreadedAVX2Aligned.data(), expected.data(), _size, "SIMD AVX2 Threaded Aligned", _tolerance);

    auto resSIMDThreadedAVX2Unaligned = createAlignedVector<T>(_size);
     VectorMath::multiply<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2, false>>(ThreadTrait, _size, a.data(), b.data(), resSIMDThreadedAVX2Unaligned.data());
    TestUtility::compareVectors<T>(resSIMDThreadedAVX2Unaligned.data(), expected.data(), _size, "SIMD AVX2 Threaded Unaligned", _tolerance);
    #endif

    #if defined(STALKER_SIMD_AVX512_OK)
    auto resSIMDThreadedAVX512Aligned = createAlignedVector<T>(_size);
     VectorMath::multiply<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512, true>>(ThreadTrait, _size, a.data(), b.data(), resSIMDThreadedAVX512Aligned.data());
    TestUtility::compareVectors<T>(resSIMDThreadedAVX512Aligned.data(), expected.data(), _size, "SIMD AVX512 Threaded Aligned", _tolerance);

    auto resSIMDThreadedAVX512Unaligned = createAlignedVector<T>(_size);
     VectorMath::multiply<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512, false>>(ThreadTrait, _size, a.data(), b.data(), resSIMDThreadedAVX512Unaligned.data());
    TestUtility::compareVectors<T>(resSIMDThreadedAVX512Unaligned.data(), expected.data(), _size, "SIMD AVX512 Threaded Unaligned", _tolerance);
    #endif

        auto resScaledClassicThreaded = createAlignedVector<T>(_size);
         VectorMath::multiply<T, ThreadTraitSTDThread, ExecutionTraitScalar<>>(ThreadTrait, _size, a.data(), b.data(), resScaledClassicThreaded.data(), scalarA, scalarB);
        TestUtility::compareVectors(resScaledClassicThreaded.data(), expectedScaled.data(), _size, "Scalar Scaled Threaded");

        auto resScaledMetaThreaded = createAlignedVector<T>(_size);
         VectorMath::multiply<T, ThreadTraitSTDThread, ExecutionTraitUnrolled<>>(ThreadTrait, _size, a.data(), b.data(), resScaledMetaThreaded.data(), scalarA, scalarB);
        TestUtility::compareVectors(resScaledMetaThreaded.data(), expectedScaled.data(), _size, "Meta Scaled Threaded");

    #if defined(STALKER_SIMD_AVX2_OK)
    auto resScaledSIMD1ThreadedAligned = createAlignedVector<T>(_size);
     VectorMath::multiply<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2, true>>(ThreadTrait, _size, a.data(), b.data(), resScaledSIMD1ThreadedAligned.data(), scalarA, scalarB);
    TestUtility::compareVectors<T>(resScaledSIMD1ThreadedAligned.data(), expectedScaled.data(), _size, "SIMD AVX2 Scaled Threaded Aligned", _tolerance);

    auto resScaledSIMD1ThreadedUnaligned = createAlignedVector<T>(_size);
     VectorMath::multiply<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2, false>>(ThreadTrait, _size, a.data(), b.data(), resScaledSIMD1ThreadedUnaligned.data(), scalarA, scalarB);
    TestUtility::compareVectors<T>(resScaledSIMD1ThreadedUnaligned.data(), expectedScaled.data(), _size, "SIMD AVX2 Scaled Threaded Unaligned", _tolerance);
    #endif

    #if defined(STALKER_SIMD_AVX512_OK)
    auto resScaledSIMD2ThreadedAligned = createAlignedVector<T>(_size);
     VectorMath::multiply<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512, true>>(ThreadTrait, _size, a.data(), b.data(), resScaledSIMD2ThreadedAligned.data(), scalarA, scalarB);
    TestUtility::compareVectors<T>(resScaledSIMD2ThreadedAligned.data(), expectedScaled.data(), _size, "SIMD AVX512 Scaled Threaded Aligned", _tolerance);

    auto resScaledSIMD2ThreadedUnaligned = createAlignedVector<T>(_size);
     VectorMath::multiply<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512, false>>(ThreadTrait, _size, a.data(), b.data(), resScaledSIMD2ThreadedUnaligned.data(), scalarA, scalarB);
    TestUtility::compareVectors<T>(resScaledSIMD2ThreadedUnaligned.data(), expectedScaled.data(), _size, "SIMD AVX512 Scaled Threaded Unaligned", _tolerance);
    #endif
        #endif
    }

    template<typename T>
    void _testSum() {
        printSubtitle("Sum " + typeToString<T>(), T_Color::BARBIE_PINK);
        auto typeName = typeToString<T>();

        auto a = createAlignedVector<T>(_size);
        Random::uniform<T>(_size, a.data(), 0, 10);

        T expected = 0;

        for (size_t i = 0; i < _size; i++) {
            expected += a[i];
        }

        {
            T resClassic =  VectorMath::sum<T, ExecutionTraitScalar<>>(_size, a.data());
            TestUtility::compareValues(resClassic, expected, "Scalar Sum", _tolerance, true);
        }

        {
            T resMeta =  VectorMath::sum<T, ExecutionTraitUnrolled<>>(_size, a.data());
            TestUtility::compareValues(resMeta, expected, "Meta Sum", _tolerance, true);
        }

        #if defined(STALKER_SIMD_AVX2_OK)
        {
            T resSIMDAligned =  VectorMath::sum<T, ExecutionTraitSIMD<T_SIMD::AVX2, true>>(_size, a.data());
            TestUtility::compareValues(resSIMDAligned, expected, "SIMD AVX2 Sum Aligned", _tolerance, true);

            T resSIMDUnaligned =  VectorMath::sum<T, ExecutionTraitSIMD<T_SIMD::AVX2, false>>(_size, a.data());
            TestUtility::compareValues(resSIMDUnaligned, expected, "SIMD AVX2 Sum Unaligned", _tolerance, true);
        }
        #endif

        #if defined(STALKER_SIMD_AVX512_OK)
        {
            T resSIMDAligned =  VectorMath::sum<T, ExecutionTraitSIMD<T_SIMD::AVX512, true>>(_size, a.data());
            TestUtility::compareValues(resSIMDAligned, expected, "SIMD AVX512 Sum Aligned", _tolerance, true);

            T resSIMDUnaligned =  VectorMath::sum<T, ExecutionTraitSIMD<T_SIMD::AVX512, false>>(_size, a.data());
            TestUtility::compareValues(resSIMDUnaligned, expected, "SIMD AVX512 Sum Unaligned", _tolerance, true);
        }
        #endif

        #if defined(STALKER_THREADING_ENABLE) && STALKER_THREADING_ENABLE != 0
        ThreadTraitSTDThread ThreadTrait = ThreadTraitSTDThread();
        auto nThreads = ThreadTrait.getNumThreads();
        auto nThreadsStr = std::to_string(nThreads);

        {
            T resClassicThreaded =  VectorMath::sum<T, ThreadTraitSTDThread, ExecutionTraitScalar<>>(ThreadTrait, _size, a.data());
            TestUtility::compareValues(resClassicThreaded, expected, "Scalar Threaded Sum", _tolerance);
        }

        {
            T resMetaThreaded =  VectorMath::sum<T, ThreadTraitSTDThread, ExecutionTraitUnrolled<>>(ThreadTrait, _size, a.data());
            TestUtility::compareValues(resMetaThreaded, expected, "Meta Threaded Sum", _tolerance);
        }

        #if defined(STALKER_SIMD_AVX2_OK)
        {
            T resSIMDThreadedAligned =  VectorMath::sum<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2, true>>(ThreadTrait, _size, a.data());
            TestUtility::compareValues(resSIMDThreadedAligned, expected, "SIMD AVX2 Threaded Sum Aligned", _tolerance);

            T resSIMDThreadedUnaligned =  VectorMath::sum<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2, false>>(ThreadTrait, _size, a.data());
            TestUtility::compareValues(resSIMDThreadedUnaligned, expected, "SIMD AVX2 Threaded Sum Unaligned", _tolerance);
        }
        #endif

        #if defined(STALKER_SIMD_AVX512_OK)
        {
            T resSIMDThreadedAligned =  VectorMath::sum<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512, true>>(ThreadTrait, _size, a.data());
            TestUtility::compareValues(resSIMDThreadedAligned, expected, "SIMD AVX512 Threaded Sum Aligned", _tolerance);

            T resSIMDThreadedUnaligned =  VectorMath::sum<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512, false>>(ThreadTrait, _size, a.data());
            TestUtility::compareValues(resSIMDThreadedUnaligned, expected, "SIMD AVX512 Threaded Sum Unaligned", _tolerance);
        }
        #endif
        #endif
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

        {
            auto a = input;
             VectorMath::scale<T, ExecutionTraitScalar<>>(_size, a.data(), scalar);
            TestUtility::compareVectors<T>(a.data(), expected.data(), _size, "Scalar", _tolerance);
        }
        {
            auto a = input;
             VectorMath::scale<T, ExecutionTraitUnrolled<>>(_size, a.data(), scalar);
            TestUtility::compareVectors<T>(a.data(), expected.data(), _size, "Meta", _tolerance);
        }
        #if defined(STALKER_SIMD_AVX2_OK)
        {
            auto a = input;
             VectorMath::scale<T, ExecutionTraitSIMD<T_SIMD::AVX2, true>>(_size, a.data(), scalar);
            TestUtility::compareVectors<T>(a.data(), expected.data(), _size, "SIMD AVX2 Aligned", _tolerance);

            a = input;
             VectorMath::scale<T, ExecutionTraitSIMD<T_SIMD::AVX2, false>>(_size, a.data(), scalar);
            TestUtility::compareVectors<T>(a.data(), expected.data(), _size, "SIMD AVX2 Unaligned", _tolerance);
        }
        #endif
        #if defined(STALKER_SIMD_AVX512_OK)
        {
            auto a = input;
             VectorMath::scale<T, ExecutionTraitSIMD<T_SIMD::AVX512, true>>(_size, a.data(), scalar);
            TestUtility::compareVectors<T>(a.data(), expected.data(), _size, "SIMD AVX512 Aligned", _tolerance);

            a = input;
             VectorMath::scale<T, ExecutionTraitSIMD<T_SIMD::AVX512, false>>(_size, a.data(), scalar);
            TestUtility::compareVectors<T>(a.data(), expected.data(), _size, "SIMD AVX512 Unaligned", _tolerance);
        }
        #endif

        #if defined(STALKER_THREADING_ENABLE) && STALKER_THREADING_ENABLE != 0
        ThreadTraitSTDThread ThreadTrait = ThreadTraitSTDThread();

        {
            auto a = input;
             VectorMath::scale<T, ThreadTraitSTDThread, ExecutionTraitScalar<>>(ThreadTrait, _size, a.data(), scalar);
            TestUtility::compareVectors<T>(a.data(), expected.data(), _size, "Scalar Threaded", _tolerance);
        }
        {
            auto a = input;
             VectorMath::scale<T, ThreadTraitSTDThread, ExecutionTraitUnrolled<>>(ThreadTrait, _size, a.data(), scalar);
            TestUtility::compareVectors<T>(a.data(), expected.data(), _size, "Meta Threaded", _tolerance);
        }
        #if defined(STALKER_SIMD_AVX2_OK)
        {
            auto a = input;
             VectorMath::scale<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2, true>>(ThreadTrait, _size, a.data(), scalar);
            TestUtility::compareVectors<T>(a.data(), expected.data(), _size, "SIMD AVX2 Threaded Aligned", _tolerance);

            a = input;
             VectorMath::scale<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2, false>>(ThreadTrait, _size, a.data(), scalar);
            TestUtility::compareVectors<T>(a.data(), expected.data(), _size, "SIMD AVX2 Threaded Unaligned", _tolerance);
        }
        #endif
        #if defined(STALKER_SIMD_AVX512_OK)
        {
            auto a = input;
             VectorMath::scale<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512, true>>(ThreadTrait, _size, a.data(), scalar);
            TestUtility::compareVectors<T>(a.data(), expected.data(), _size, "SIMD AVX512 Threaded Aligned", _tolerance);

            a = input;
             VectorMath::scale<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512, false>>(ThreadTrait, _size, a.data(), scalar);
            TestUtility::compareVectors<T>(a.data(), expected.data(), _size, "SIMD AVX512 Threaded Unaligned", _tolerance);
        }
        #endif
        #endif
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

        {
            auto a = input;
             VectorMath::addConstant<T, ExecutionTraitScalar<>>(_size, a.data(), scalar);
            TestUtility::compareVectors<T>(a.data(), expected.data(), _size, "Scalar", _tolerance);
        }
        {
            auto a = input;
             VectorMath::addConstant<T, ExecutionTraitUnrolled<>>(_size, a.data(), scalar);
            TestUtility::compareVectors<T>(a.data(), expected.data(), _size, "Meta", _tolerance);
        }
        #if defined(STALKER_SIMD_AVX2_OK)
        {
            auto a = input;
             VectorMath::addConstant<T, ExecutionTraitSIMD<T_SIMD::AVX2, true>>(_size, a.data(), scalar);
            TestUtility::compareVectors<T>(a.data(), expected.data(), _size, "SIMD AVX2 Aligned", _tolerance);

            a = input;
             VectorMath::addConstant<T, ExecutionTraitSIMD<T_SIMD::AVX2, false>>(_size, a.data(), scalar);
            TestUtility::compareVectors<T>(a.data(), expected.data(), _size, "SIMD AVX2 Unaligned", _tolerance);
        }
        #endif
        #if defined(STALKER_SIMD_AVX512_OK)
        {
            auto a = input;
             VectorMath::addConstant<T, ExecutionTraitSIMD<T_SIMD::AVX512, true>>(_size, a.data(), scalar);
            TestUtility::compareVectors<T>(a.data(), expected.data(), _size, "SIMD AVX512 Aligned", _tolerance);

            a = input;
             VectorMath::addConstant<T, ExecutionTraitSIMD<T_SIMD::AVX512, false>>(_size, a.data(), scalar);
            TestUtility::compareVectors<T>(a.data(), expected.data(), _size, "SIMD AVX512 Unaligned", _tolerance);
        }
        #endif

        #if defined(STALKER_THREADING_ENABLE) && STALKER_THREADING_ENABLE != 0
        ThreadTraitSTDThread ThreadTrait = ThreadTraitSTDThread();

        {
            auto a = input;
             VectorMath::addConstant<T, ThreadTraitSTDThread, ExecutionTraitScalar<>>(ThreadTrait, _size, a.data(), scalar);
            TestUtility::compareVectors<T>(a.data(), expected.data(), _size, "Scalar Threaded", _tolerance);
        }
        {
            auto a = input;
             VectorMath::addConstant<T, ThreadTraitSTDThread, ExecutionTraitUnrolled<>>(ThreadTrait, _size, a.data(), scalar);
            TestUtility::compareVectors<T>(a.data(), expected.data(), _size, "Meta Threaded", _tolerance);
        }
        #if defined(STALKER_SIMD_AVX2_OK)
        {
            auto a = input;
             VectorMath::addConstant<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2, true>>(ThreadTrait, _size, a.data(), scalar);
            TestUtility::compareVectors<T>(a.data(), expected.data(), _size, "SIMD AVX2 Threaded Aligned", _tolerance);

            a = input;
             VectorMath::addConstant<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2, false>>(ThreadTrait, _size, a.data(), scalar);
            TestUtility::compareVectors<T>(a.data(), expected.data(), _size, "SIMD AVX2 Threaded Unaligned", _tolerance);
        }
        #endif
        #if defined(STALKER_SIMD_AVX512_OK)
        {
            auto a = input;
             VectorMath::addConstant<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512, true>>(ThreadTrait, _size, a.data(), scalar);
            TestUtility::compareVectors<T>(a.data(), expected.data(), _size, "SIMD AVX512 Threaded Aligned", _tolerance);

            a = input;
             VectorMath::addConstant<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512, false>>(ThreadTrait, _size, a.data(), scalar);
            TestUtility::compareVectors<T>(a.data(), expected.data(), _size, "SIMD AVX512 Threaded Unaligned", _tolerance);
        }
        #endif
        #endif
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

        double tolerance = 1e-4;
        if constexpr (std::is_same_v<T, float>)
            tolerance = 5e-2;
        {
            auto res =  VectorMath::dot<T, ExecutionTraitScalar<>>(_size, a.data(), b.data());
            TestUtility::compareValues<T>(res, expected, "Scalar", tolerance, true);
        }
        {
            auto res =  VectorMath::dot<T, ExecutionTraitUnrolled<1>>(_size, a.data(), b.data());
            TestUtility::compareValues<T>(res, expected, "Meta", tolerance, true);
        }
        #if defined(STALKER_SIMD_AVX2_OK)
        {
            auto resAligned =  VectorMath::dot<T, ExecutionTraitSIMD<T_SIMD::AVX2, true>>(_size, a.data(), b.data());
            TestUtility::compareValues<T>(resAligned, expected, "SIMD AVX2 Aligned", tolerance, true);

            auto resUnaligned =  VectorMath::dot<T, ExecutionTraitSIMD<T_SIMD::AVX2, false>>(_size, a.data(), b.data());
            TestUtility::compareValues<T>(resUnaligned, expected, "SIMD AVX2 Unaligned", tolerance, true);
        }
        #endif
        #if defined(STALKER_SIMD_AVX512_OK)
        {
            auto resAligned =  VectorMath::dot<T, ExecutionTraitSIMD<T_SIMD::AVX512, true>>(_size, a.data(), b.data());
            TestUtility::compareValues<T>(resAligned, expected, "SIMD AVX512 Aligned", tolerance, true);

            auto resUnaligned =  VectorMath::dot<T, ExecutionTraitSIMD<T_SIMD::AVX512, false>>(_size, a.data(), b.data());
            TestUtility::compareValues<T>(resUnaligned, expected, "SIMD AVX512 Unaligned", tolerance, true);
        }
        #endif

        #if defined(STALKER_THREADING_ENABLE) && STALKER_THREADING_ENABLE != 0
        ThreadTraitSTDThread ThreadTrait = ThreadTraitSTDThread();

        {
            auto res =  VectorMath::dot<T, ThreadTraitSTDThread, ExecutionTraitScalar<>>(ThreadTrait, _size, a.data(), b.data());
            TestUtility::compareValues<T>(res, expected, "Scalar Threaded", tolerance, true);
        }
        {
            auto res =  VectorMath::dot<T, ThreadTraitSTDThread, ExecutionTraitUnrolled<>>(ThreadTrait, _size, a.data(), b.data());
            TestUtility::compareValues<T>(res, expected, "Meta Threaded", tolerance, true);
        }
        #if defined(STALKER_SIMD_AVX2_OK)
        {
            auto resAligned =  VectorMath::dot<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2, true>>(ThreadTrait, _size, a.data(), b.data());
            TestUtility::compareValues<T>(resAligned, expected, "SIMD AVX2 Threaded Aligned", tolerance, true);

            auto resUnaligned =  VectorMath::dot<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2, false>>(ThreadTrait, _size, a.data(), b.data());
            TestUtility::compareValues<T>(resUnaligned, expected, "SIMD AVX2 Threaded Unaligned", tolerance, true);
        }
        #endif
        #if defined(STALKER_SIMD_AVX512_OK)
        {
            auto resAligned =  VectorMath::dot<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512, true>>(ThreadTrait, _size, a.data(), b.data());
            TestUtility::compareValues<T>(resAligned, expected, "SIMD AVX512 Threaded Aligned", tolerance, true);

            auto resUnaligned =  VectorMath::dot<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512, false>>(ThreadTrait, _size, a.data(), b.data());
            TestUtility::compareValues<T>(resUnaligned, expected, "SIMD AVX512 Threaded Unaligned", tolerance, true);
        }
        #endif
        #endif
    }
};


} // namespace STLKR_Tests