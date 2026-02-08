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

#include "../STLKR_TestBase.h"

namespace STLKR_Tests {

    using namespace Stalker::Memory;
    
    class MemoryOperationsTests : public STLKR_TestBase {
    public:
        explicit MemoryOperationsTests(size_t size = 1024) : STLKR_TestBase("Memory Operations Validity Tests"), _size(size) { }

        void runTest() override {
            _copy<double>();
            _copy<float>();
            _copy<int>();
            _copy<unsigned>();
            _copy<short>();

            _setValue<double>();
            _setValue<float>();
            _setValue<int>();
            _setValue<unsigned>();
            _setValue<short>();

            _setZero<double>();
            _setZero<float>();
            _setZero<int>();
            _setZero<unsigned>();
            _setZero<short>();
        }

    private:
        
        size_t _size;
        
        template <typename T>
        void _copy(){

            printSubtitle("Copy " + TestUtility::getTypeString<T>(), T_Color::BARBIE_PINK);
            auto data = createAlignedUniquePtr<T>(_size);
            Random::uniform<T>(_size, data.get(), 0, 10);
            string name;

            {
                name = {"Scalar (Loop)"};
                auto result = createAlignedUniquePtr<T>(_size);
                constexpr bool IsSTD = false;
                MemoryOperations::copy<T, ExecutionTraitScalar<IsSTD>>(_size, result.get(), data.get());
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }

            {
                name = {"Scalar (memcpy)"};
                auto result = createAlignedUniquePtr<T>(_size);
                constexpr bool IsSTD = true;
                MemoryOperations::copy<T, ExecutionTraitScalar<IsSTD>>(_size, result.get(), data.get());
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }

            {
                name = {"Meta"};
                auto result = createAlignedUniquePtr<T>(_size);
                MemoryOperations::copy<T, ExecutionTraitUnrolled<>>(_size, result.get(), data.get());
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }
            
            #if defined(STALKER_SIMD_AVX2_OK)
            {
                name = {"SIMD_AVX2_Aligned_Cached"};
                auto result = createAlignedUniquePtr<T>(_size);
                MemoryOperations::copy<T, ExecutionTraitSIMD<T_SIMD::AVX2, true, T_SIMDStore::Cached, DefaultUnroll()>>(_size, result.get(), data.get());
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }

            {
                name = {"SIMD_AVX2_Aligned_Cached_PrefetchT0"};
                auto result = createAlignedUniquePtr<T>(_size);
                MemoryOperations::copy<T, ExecutionTraitSIMD<T_SIMD::AVX2, true, T_SIMDStore::Cached, DefaultUnroll(), T_PrefetchHints::HintT0>>(_size, result.get(), data.get());
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }

            {
                name = {"SIMD_AVX2_Unaligned_Cached"};
                auto result = createAlignedUniquePtr<T>(_size);
                MemoryOperations::copy<T, ExecutionTraitSIMD<T_SIMD::AVX2, false, T_SIMDStore::Cached, DefaultUnroll()>>(_size, result.get(), data.get());
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }

            {
                name = {"SIMD_AVX2_Aligned_Streamed"};
                auto result = createAlignedUniquePtr<T>(_size);
                MemoryOperations::copy<T, ExecutionTraitSIMD<T_SIMD::AVX2, true, T_SIMDStore::Streamed, DefaultUnroll()>>(_size, result.get(), data.get());
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }

            {
                name = {"SIMD_AVX2_Unaligned_Streamed"};
                auto result = createAlignedUniquePtr<T>(_size);
                MemoryOperations::copy<T, ExecutionTraitSIMD<T_SIMD::AVX2, false, T_SIMDStore::Streamed, DefaultUnroll()>>(_size, result.get(), data.get());
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }
            #endif

            #if defined(STALKER_SIMD_AVX512_OK)
            {
                name = {"SIMD_AVX512_Aligned_Cached"};
                auto result = createAlignedUniquePtr<T>(_size);
                MemoryOperations::copy<T, ExecutionTraitSIMD<T_SIMD::AVX512, true, T_SIMDStore::Cached, DefaultUnroll()>>(_size, result.get(), data.get());
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }

            {
                name = {"SIMD_AVX512_Unaligned_Cached"};
                auto result = createAlignedUniquePtr<T>(_size);
                MemoryOperations::copy<T, ExecutionTraitSIMD<T_SIMD::AVX512, false, T_SIMDStore::Cached, DefaultUnroll()>>(_size, result.get(), data.get());
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }

            {
                name = {"SIMD_AVX512_Aligned_Streamed"};
                auto result = createAlignedUniquePtr<T>(_size);
                MemoryOperations::copy<T, ExecutionTraitSIMD<T_SIMD::AVX512, true, T_SIMDStore::Streamed, DefaultUnroll()>>(_size, result.get(), data.get());
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }

            {
                name = {"SIMD_AVX512_Unaligned_Streamed"};
                auto result = createAlignedUniquePtr<T>(_size);
                MemoryOperations::copy<T, ExecutionTraitSIMD<T_SIMD::AVX512, false, T_SIMDStore::Streamed, DefaultUnroll()>>(_size, result.get(), data.get());
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }
            #endif

            #if defined(STALKER_THREADING_ENABLE) && STALKER_THREADING_ENABLE != 0

            ThreadTraitSTDThread threadTrait = ThreadTraitSTDThread();
            auto nThreads = threadTrait.getNumThreads();
            auto nThreadsStr = std::to_string(nThreads);

            {
                name = {"Parallel_Classic (Loop)"};
                auto result = createAlignedUniquePtr<T>(_size);
                constexpr bool IsSTD = false;
                MemoryOperations::copy<T, ThreadTraitSTDThread, ExecutionTraitScalar<IsSTD>>(threadTrait, _size, result.get(), data.get());
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }

            {
                name = {"Parallel_Classic (memcpy)"};
                auto result = createAlignedUniquePtr<T>(_size);
                constexpr bool IsSTD = true;
                MemoryOperations::copy<T, ThreadTraitSTDThread, ExecutionTraitScalar<IsSTD>>(threadTrait, _size, result.get(), data.get());
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }

            {
                name = {"Parallel_Meta"};
                auto result = createAlignedUniquePtr<T>(_size);
                MemoryOperations::copy<T, ThreadTraitSTDThread, ExecutionTraitUnrolled<>>(threadTrait, _size, result.get(), data.get());
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }
            
            #if defined(STALKER_SIMD_AVX2_OK)
            {
                name = {"Parallel_SIMD_AVX2_Aligned_Cached"};
                auto result = createAlignedUniquePtr<T>(_size);
                MemoryOperations::copy<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2, true, T_SIMDStore::Cached, DefaultUnroll()>>(threadTrait, _size, result.get(), data.get());
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }

            {
                name = {"Parallel_SIMD_AVX2_Unaligned_Cached"};
                auto result = createAlignedUniquePtr<T>(_size);
                MemoryOperations::copy<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2, false, T_SIMDStore::Cached, DefaultUnroll()>>(threadTrait, _size, result.get(), data.get());
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }

            {
                name = {"Parallel_SIMD_AVX2_Aligned_Streamed"};
                auto result = createAlignedUniquePtr<T>(_size);
                MemoryOperations::copy<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2, true, T_SIMDStore::Streamed, DefaultUnroll()>>(threadTrait, _size, result.get(), data.get());
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }

            {
                name = {"Parallel_SIMD_AVX2_Unaligned_Streamed"};
                auto result = createAlignedUniquePtr<T>(_size);
                MemoryOperations::copy<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2, false, T_SIMDStore::Streamed, DefaultUnroll()>>(threadTrait, _size, result.get(), data.get());
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }
            #endif

            #if defined(STALKER_SIMD_AVX512_OK)
            {
                name = {"Parallel_SIMD_AVX512_Aligned_Cached"};
                auto result = createAlignedUniquePtr<T>(_size);
                MemoryOperations::copy<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512, true, T_SIMDStore::Cached, DefaultUnroll()>>(threadTrait, _size, result.get(), data.get());
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }

            {
                name = {"Parallel_SIMD_AVX512_Unaligned_Cached"};
                auto result = createAlignedUniquePtr<T>(_size);
                MemoryOperations::copy<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512, false, T_SIMDStore::Cached, DefaultUnroll()>>(threadTrait, _size, result.get(), data.get());
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }

            {
                name = {"Parallel_SIMD_AVX512_Aligned_Streamed"};
                auto result = createAlignedUniquePtr<T>(_size);
                MemoryOperations::copy<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512, true, T_SIMDStore::Streamed, DefaultUnroll()>>(threadTrait, _size, result.get(), data.get());
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }

            {
                name = {"Parallel_SIMD_AVX512_Unaligned_Streamed"};
                auto result = createAlignedUniquePtr<T>(_size);
                MemoryOperations::copy<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512, false, T_SIMDStore::Streamed, DefaultUnroll()>>(threadTrait, _size, result.get(), data.get());
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }
            #endif

            #endif
        
        }

        template <typename T>
        void _setValue(){

            printSubtitle("SetValue " + TestUtility::getTypeString<T>(), T_Color::BARBIE_PINK);
            auto data = createAlignedUniquePtr<T>(_size);
            T value = 3;
            for (size_t i = 0; i < _size; ++i){
                data.get()[i] = value;
            }
            string name;

            {
                name = {"Scalar (Loop)"};
                auto result = createAlignedUniquePtr<T>(_size);
                constexpr bool IsSTD = false;
                MemoryOperations::setValue<T, ExecutionTraitScalar<IsSTD>>(_size, result.get(), value);
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }

            {
                name = {"Scalar (std::fill)"};
                auto result = createAlignedUniquePtr<T>(_size);
                constexpr bool IsSTD = true;
                MemoryOperations::setValue<T, ExecutionTraitScalar<IsSTD>>(_size, result.get(), value);
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }

            {
                name = {"Meta"};
                auto result = createAlignedUniquePtr<T>(_size);
                MemoryOperations::setValue<T, ExecutionTraitUnrolled<>>(_size, result.get(), value);
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }
            
            #if defined(STALKER_SIMD_AVX2_OK)
            {
                name = {"SIMD_AVX2_Aligned_Cached"};
                auto result = createAlignedUniquePtr<T>(_size);
                MemoryOperations::setValue<T, ExecutionTraitSIMD<T_SIMD::AVX2, true, T_SIMDStore::Cached, DefaultUnroll()>>(_size, result.get(), value);
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }

            {
                name = {"SIMD_AVX2_Unaligned_Cached"};
                auto result = createAlignedUniquePtr<T>(_size);
                MemoryOperations::setValue<T, ExecutionTraitSIMD<T_SIMD::AVX2, false, T_SIMDStore::Cached, DefaultUnroll()>>(_size, result.get(), value);
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }

            {
                name = {"SIMD_AVX2_Aligned_Streamed"};
                auto result = createAlignedUniquePtr<T>(_size);
                MemoryOperations::setValue<T, ExecutionTraitSIMD<T_SIMD::AVX2, true, T_SIMDStore::Streamed, DefaultUnroll()>>(_size, result.get(), value);
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }

            {
                name = {"SIMD_AVX2_Unaligned_Streamed"};
                auto result = createAlignedUniquePtr<T>(_size);
                MemoryOperations::setValue<T, ExecutionTraitSIMD<T_SIMD::AVX2, false, T_SIMDStore::Streamed, DefaultUnroll()>>(_size, result.get(), value);
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }
            #endif

            #if defined(STALKER_SIMD_AVX512_OK)
            {
                name = {"SIMD_AVX512_Aligned_Cached"};
                auto result = createAlignedUniquePtr<T>(_size);
                MemoryOperations::setValue<T, ExecutionTraitSIMD<T_SIMD::AVX512, true, T_SIMDStore::Cached, DefaultUnroll()>>(_size, result.get(), value);
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }

            {
                name = {"SIMD_AVX512_Unaligned_Cached"};
                auto result = createAlignedUniquePtr<T>(_size);
                MemoryOperations::setValue<T, ExecutionTraitSIMD<T_SIMD::AVX512, false, T_SIMDStore::Cached, DefaultUnroll()>>(_size, result.get(), value);
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }

            {
                name = {"SIMD_AVX512_Aligned_Streamed"};
                auto result = createAlignedUniquePtr<T>(_size);
                MemoryOperations::setValue<T, ExecutionTraitSIMD<T_SIMD::AVX512, true, T_SIMDStore::Streamed, DefaultUnroll()>>(_size, result.get(), value);
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }

            {
                name = {"SIMD_AVX512_Unaligned_Streamed"};
                auto result = createAlignedUniquePtr<T>(_size);
                MemoryOperations::setValue<T, ExecutionTraitSIMD<T_SIMD::AVX512, false, T_SIMDStore::Streamed, DefaultUnroll()>>(_size, result.get(), value);
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }
            #endif

            #if defined(STALKER_THREADING_ENABLE) && STALKER_THREADING_ENABLE != 0

            ThreadTraitSTDThread threadTrait = ThreadTraitSTDThread();
            auto nThreads = threadTrait.getNumThreads();
            auto nThreadsStr = std::to_string(nThreads);

            {
                name = {"Parallel_Classic (Loop)"};
                auto result = createAlignedUniquePtr<T>(_size);
                constexpr bool IsSTD = false;
                MemoryOperations::setValue<T, ThreadTraitSTDThread, ExecutionTraitScalar<IsSTD>>(threadTrait, _size, result.get(), value);
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }

            {
                name = {"Parallel_Classic (std::fill)"};
                auto result = createAlignedUniquePtr<T>(_size);
                constexpr bool IsSTD = true;
                MemoryOperations::setValue<T, ThreadTraitSTDThread, ExecutionTraitScalar<IsSTD>>(threadTrait, _size, result.get(), value);
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }

            {
                name = {"Parallel_Meta"};
                auto result = createAlignedUniquePtr<T>(_size);
                MemoryOperations::setValue<T, ThreadTraitSTDThread, ExecutionTraitUnrolled<>>(threadTrait, _size, result.get(), value);
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }
            
            #if defined(STALKER_SIMD_AVX2_OK)
            {
                name = {"Parallel_SIMD_AVX2_Aligned_Cached"};
                auto result = createAlignedUniquePtr<T>(_size);
                MemoryOperations::setValue<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2, true, T_SIMDStore::Cached, DefaultUnroll()>>(threadTrait, _size, result.get(), value);
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }

            {
                name = {"Parallel_SIMD_AVX2_Unaligned_Cached"};
                auto result = createAlignedUniquePtr<T>(_size);
                MemoryOperations::setValue<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2, false, T_SIMDStore::Cached, DefaultUnroll()>>(threadTrait, _size, result.get(), value);
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }

            {
                name = {"Parallel_SIMD_AVX2_Aligned_Streamed"};
                auto result = createAlignedUniquePtr<T>(_size);
                MemoryOperations::setValue<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2, true, T_SIMDStore::Streamed, DefaultUnroll()>>(threadTrait, _size, result.get(), value);
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }

            {
                name = {"Parallel_SIMD_AVX2_Unaligned_Streamed"};
                auto result = createAlignedUniquePtr<T>(_size);
                MemoryOperations::setValue<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2, false, T_SIMDStore::Streamed, DefaultUnroll()>>(threadTrait, _size, result.get(), value);
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }
            #endif

            #if defined(STALKER_SIMD_AVX512_OK)
            {
                name = {"Parallel_SIMD_AVX512_Aligned_Cached"};
                auto result = createAlignedUniquePtr<T>(_size);
                MemoryOperations::setValue<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512, true, T_SIMDStore::Cached, DefaultUnroll()>>(threadTrait, _size, result.get(), value);
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }

            {
                name = {"Parallel_SIMD_AVX512_Unaligned_Cached"};
                auto result = createAlignedUniquePtr<T>(_size);
                MemoryOperations::setValue<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512, false, T_SIMDStore::Cached, DefaultUnroll()>>(threadTrait, _size, result.get(), value);
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }

            {
                name = {"Parallel_SIMD_AVX512_Aligned_Streamed"};
                auto result = createAlignedUniquePtr<T>(_size);
                MemoryOperations::setValue<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512, true, T_SIMDStore::Streamed, DefaultUnroll()>>(threadTrait, _size, result.get(), value);
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }

            {
                name = {"Parallel_SIMD_AVX512_Unaligned_Streamed"};
                auto result = createAlignedUniquePtr<T>(_size);
                MemoryOperations::setValue<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512, false, T_SIMDStore::Streamed, DefaultUnroll()>>(threadTrait, _size, result.get(), value);
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }
            #endif
            #endif
        
        }

        template <typename T>
        void _setZero(){

            printSubtitle("SetZero " + TestUtility::getTypeString<T>(), T_Color::BARBIE_PINK);
            auto data = createAlignedUniquePtr<T>(_size);
            T value = 0;
            for (size_t i = 0; i < _size; ++i){
                data.get()[i] = value;
            }
            string name;

            {
                name = {"Scalar (Loop)"};
                auto result = createAlignedUniquePtr<T>(_size);
                constexpr bool IsSTD = false;
                MemoryOperations::setZero<T, ExecutionTraitScalar<IsSTD>>(_size, result.get());
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }

            {
                name = {"Scalar (memset)"};
                auto result = createAlignedUniquePtr<T>(_size);
                constexpr bool IsSTD = true;
                MemoryOperations::setZero<T, ExecutionTraitScalar<IsSTD>>(_size, result.get());
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }

            {
                name = {"Meta"};
                auto result = createAlignedUniquePtr<T>(_size);
                MemoryOperations::setZero<T, ExecutionTraitUnrolled<>>(_size, result.get());
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }
            
            #if defined(STALKER_SIMD_AVX2_OK)
            {
                name = {"SIMD_AVX2_Aligned_Cached"};
                auto result = createAlignedUniquePtr<T>(_size);
                MemoryOperations::setZero<T, ExecutionTraitSIMD<T_SIMD::AVX2, true, T_SIMDStore::Cached, DefaultUnroll()>>(_size, result.get());
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }

            {
                name = {"SIMD_AVX2_Unaligned_Cached"};
                auto result = createAlignedUniquePtr<T>(_size);
                MemoryOperations::setZero<T, ExecutionTraitSIMD<T_SIMD::AVX2, false, T_SIMDStore::Cached, DefaultUnroll()>>(_size, result.get());
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }

            {
                name = {"SIMD_AVX2_Aligned_Streamed"};
                auto result = createAlignedUniquePtr<T>(_size);
                MemoryOperations::setZero<T, ExecutionTraitSIMD<T_SIMD::AVX2, true, T_SIMDStore::Streamed, DefaultUnroll()>>(_size, result.get());
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }

            {
                name = {"SIMD_AVX2_Unaligned_Streamed"};
                auto result = createAlignedUniquePtr<T>(_size);
                MemoryOperations::setZero<T, ExecutionTraitSIMD<T_SIMD::AVX2, false, T_SIMDStore::Streamed, DefaultUnroll()>>(_size, result.get());
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }
            #endif

            #if defined(STALKER_SIMD_AVX512_OK)
            {
                name = {"SIMD_AVX512_Aligned_Cached"};
                auto result = createAlignedUniquePtr<T>(_size);
                MemoryOperations::setZero<T, ExecutionTraitSIMD<T_SIMD::AVX512, true, T_SIMDStore::Cached, DefaultUnroll()>>(_size, result.get());
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }

            {
                name = {"SIMD_AVX512_Unaligned_Cached"};
                auto result = createAlignedUniquePtr<T>(_size);
                MemoryOperations::setZero<T, ExecutionTraitSIMD<T_SIMD::AVX512, false, T_SIMDStore::Cached, DefaultUnroll()>>(_size, result.get());
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }

            {
                name = {"SIMD_AVX512_Aligned_Streamed"};
                auto result = createAlignedUniquePtr<T>(_size);
                MemoryOperations::setZero<T, ExecutionTraitSIMD<T_SIMD::AVX512, true, T_SIMDStore::Streamed, DefaultUnroll()>>(_size, result.get());
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }

            {
                name = {"SIMD_AVX512_Unaligned_Streamed"};
                auto result = createAlignedUniquePtr<T>(_size);
                MemoryOperations::setZero<T, ExecutionTraitSIMD<T_SIMD::AVX512, false, T_SIMDStore::Streamed, DefaultUnroll()>>(_size, result.get());
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }
            #endif

            #if defined(STALKER_THREADING_ENABLE) && STALKER_THREADING_ENABLE != 0

            ThreadTraitSTDThread threadTrait = ThreadTraitSTDThread();
            auto nThreads = threadTrait.getNumThreads();
            auto nThreadsStr = std::to_string(nThreads);
            
            {
                name = {"ParallelClassic (Loop)"};
                auto result = createAlignedUniquePtr<T>(_size);
                constexpr bool IsSTD = false;
                MemoryOperations::setZero<T, ThreadTraitSTDThread, ExecutionTraitScalar<IsSTD>>(threadTrait, _size, result.get());
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }

            {
                name = {"ParallelClassic (memset)"};
                auto result = createAlignedUniquePtr<T>(_size);
                constexpr bool IsSTD = true;
                MemoryOperations::setZero<T, ThreadTraitSTDThread, ExecutionTraitScalar<IsSTD>>(threadTrait, _size, result.get());
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }

            {
                name = {"ParallelMeta"};
                auto result = createAlignedUniquePtr<T>(_size);
                MemoryOperations::setZero<T, ThreadTraitSTDThread, ExecutionTraitUnrolled<>>(threadTrait, _size, result.get());
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }
            
            #if defined(STALKER_SIMD_AVX2_OK)
            {
                name = {"ParallelSIMD_AVX2_Aligned_Cached"};
                auto result = createAlignedUniquePtr<T>(_size);
                MemoryOperations::setZero<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2, true, T_SIMDStore::Cached, DefaultUnroll()>>(threadTrait, _size, result.get());
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }

            {
                name = {"ParallelSIMD_AVX2_Unaligned_Cached"};
                auto result = createAlignedUniquePtr<T>(_size);
                MemoryOperations::setZero<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2, false, T_SIMDStore::Cached, DefaultUnroll()>>(threadTrait, _size, result.get());
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }

            {
                name = {"ParallelSIMD_AVX2_Aligned_Streamed"};
                auto result = createAlignedUniquePtr<T>(_size);
                MemoryOperations::setZero<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2, true, T_SIMDStore::Streamed, DefaultUnroll()>>(threadTrait, _size, result.get());
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }

            {
                name = {"ParallelSIMD_AVX2_Unaligned_Streamed"};
                auto result = createAlignedUniquePtr<T>(_size);
                MemoryOperations::setZero<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2, false, T_SIMDStore::Streamed, DefaultUnroll()>>(threadTrait, _size, result.get());
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }
            #endif

            #if defined(STALKER_SIMD_AVX512_OK)
            {
                name = {"ParallelSIMD_AVX512_Aligned_Cached"};
                auto result = createAlignedUniquePtr<T>(_size);
                MemoryOperations::setZero<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512, true, T_SIMDStore::Cached, DefaultUnroll()>>(threadTrait, _size, result.get());
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }

            {
                name = {"ParallelSIMD_AVX512_Unaligned_Cached"};
                auto result = createAlignedUniquePtr<T>(_size);
                MemoryOperations::setZero<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512, false, T_SIMDStore::Cached, DefaultUnroll()>>(threadTrait, _size, result.get());
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }

            {
                name = {"ParallelSIMD_AVX512_Aligned_Streamed"};
                auto result = createAlignedUniquePtr<T>(_size);
                MemoryOperations::setZero<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512, true, T_SIMDStore::Streamed, DefaultUnroll()>>(threadTrait, _size, result.get());
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }

            {
                name = {"ParallelSIMD_AVX512_Unaligned_Streamed"};
                auto result = createAlignedUniquePtr<T>(_size);
                MemoryOperations::setZero<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512, false, T_SIMDStore::Streamed, DefaultUnroll()>>(threadTrait, _size, result.get());
                TestUtility::compareVectors<T>(result.get(), data.get(), _size, name);
            }
            #endif
            #endif
        
        }

    };

} // namespace STLKR_Tests