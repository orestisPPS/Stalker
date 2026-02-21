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

            _swap<double>();
            _swap<float>();
            _swap<int>();
            _swap<unsigned>();
            _swap<short>();
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
        void _swap(){

            printSubtitle("Swap " + TestUtility::getTypeString<T>(), T_Color::BARBIE_PINK);
            auto data1 = createAlignedUniquePtr<T>(_size);
            auto data2 = createAlignedUniquePtr<T>(_size);
            
            // Initialize data1 with some values (e.g. 1s) and data2 with others (e.g. 2s)
            Random::uniform<T>(_size, data1.get(), 1, 1); 
            Random::uniform<T>(_size, data2.get(), 2, 2);

            // We want to verify that after swap: data1 has 2s, data2 has 1s
            auto expected_in_data1 = createAlignedUniquePtr<T>(_size);
            auto expected_in_data2 = createAlignedUniquePtr<T>(_size);
            // Copy initial states to expected buffers, but swapped
            std::memcpy(expected_in_data1.get(), data2.get(), _size * sizeof(T));
            std::memcpy(expected_in_data2.get(), data1.get(), _size * sizeof(T));

            string name;

            {
                name = {"Scalar (Loop)"};
                auto d1 = createAlignedUniquePtr<T>(_size);
                auto d2 = createAlignedUniquePtr<T>(_size);
                std::memcpy(d1.get(), data1.get(), _size * sizeof(T));
                std::memcpy(d2.get(), data2.get(), _size * sizeof(T));

                constexpr bool IsSTD = false;
                MemoryOperations::swap<T, ExecutionTraitScalar<IsSTD>>(_size, d1.get(), d2.get());
                
                TestUtility::compareVectors<T>(d1.get(), expected_in_data1.get(), _size, name + " d1");
                TestUtility::compareVectors<T>(d2.get(), expected_in_data2.get(), _size, name + " d2");
            }

            {
                name = {"Scalar (std::swap_ranges)"};
                auto d1 = createAlignedUniquePtr<T>(_size);
                auto d2 = createAlignedUniquePtr<T>(_size);
                std::memcpy(d1.get(), data1.get(), _size * sizeof(T));
                std::memcpy(d2.get(), data2.get(), _size * sizeof(T));
                
                constexpr bool IsSTD = true;
                MemoryOperations::swap<T, ExecutionTraitScalar<IsSTD>>(_size, d1.get(), d2.get());
                
                TestUtility::compareVectors<T>(d1.get(), expected_in_data1.get(), _size, name + " d1");
                TestUtility::compareVectors<T>(d2.get(), expected_in_data2.get(), _size, name + " d2");
            }

            {
                name = {"Meta"};
                auto d1 = createAlignedUniquePtr<T>(_size);
                auto d2 = createAlignedUniquePtr<T>(_size);
                std::memcpy(d1.get(), data1.get(), _size * sizeof(T));
                std::memcpy(d2.get(), data2.get(), _size * sizeof(T));

                MemoryOperations::swap<T, ExecutionTraitUnrolled<>>(_size, d1.get(), d2.get());

                TestUtility::compareVectors<T>(d1.get(), expected_in_data1.get(), _size, name + " d1");
                TestUtility::compareVectors<T>(d2.get(), expected_in_data2.get(), _size, name + " d2");
            }
            
            #if defined(STALKER_SIMD_AVX2_OK)
            {
                name = {"SIMD_AVX2_Aligned_Cached"};
                auto d1 = createAlignedUniquePtr<T>(_size);
                auto d2 = createAlignedUniquePtr<T>(_size);
                std::memcpy(d1.get(), data1.get(), _size * sizeof(T));
                std::memcpy(d2.get(), data2.get(), _size * sizeof(T));

                MemoryOperations::swap<T, ExecutionTraitSIMD<T_SIMD::AVX2, true, T_SIMDStore::Cached, DefaultUnroll()>>(_size, d1.get(), d2.get());
                
                TestUtility::compareVectors<T>(d1.get(), expected_in_data1.get(), _size, name + " d1");
                TestUtility::compareVectors<T>(d2.get(), expected_in_data2.get(), _size, name + " d2");
            }

            {
                name = {"SIMD_AVX2_Unaligned_Cached"};
                auto d1 = createAlignedUniquePtr<T>(_size);
                auto d2 = createAlignedUniquePtr<T>(_size);
                std::memcpy(d1.get(), data1.get(), _size * sizeof(T));
                std::memcpy(d2.get(), data2.get(), _size * sizeof(T));

                MemoryOperations::swap<T, ExecutionTraitSIMD<T_SIMD::AVX2, false, T_SIMDStore::Cached, DefaultUnroll()>>(_size, d1.get(), d2.get());

                TestUtility::compareVectors<T>(d1.get(), expected_in_data1.get(), _size, name + " d1");
                TestUtility::compareVectors<T>(d2.get(), expected_in_data2.get(), _size, name + " d2");
            }

            {
                name = {"SIMD_AVX2_Aligned_Streamed"};
                auto d1 = createAlignedUniquePtr<T>(_size);
                auto d2 = createAlignedUniquePtr<T>(_size);
                std::memcpy(d1.get(), data1.get(), _size * sizeof(T));
                std::memcpy(d2.get(), data2.get(), _size * sizeof(T));

                MemoryOperations::swap<T, ExecutionTraitSIMD<T_SIMD::AVX2, true, T_SIMDStore::Streamed, DefaultUnroll()>>(_size, d1.get(), d2.get());

                TestUtility::compareVectors<T>(d1.get(), expected_in_data1.get(), _size, name + " d1");
                TestUtility::compareVectors<T>(d2.get(), expected_in_data2.get(), _size, name + " d2");
            }

            {
                name = {"SIMD_AVX2_Unaligned_Streamed"};
                auto d1 = createAlignedUniquePtr<T>(_size);
                auto d2 = createAlignedUniquePtr<T>(_size);
                std::memcpy(d1.get(), data1.get(), _size * sizeof(T));
                std::memcpy(d2.get(), data2.get(), _size * sizeof(T));

                MemoryOperations::swap<T, ExecutionTraitSIMD<T_SIMD::AVX2, false, T_SIMDStore::Streamed, DefaultUnroll()>>(_size, d1.get(), d2.get());

                TestUtility::compareVectors<T>(d1.get(), expected_in_data1.get(), _size, name + " d1");
                TestUtility::compareVectors<T>(d2.get(), expected_in_data2.get(), _size, name + " d2");
            }
            #endif

            #if defined(STALKER_SIMD_AVX512_OK)
            {
                name = {"SIMD_AVX512_Aligned_Cached"};
                auto d1 = createAlignedUniquePtr<T>(_size);
                auto d2 = createAlignedUniquePtr<T>(_size);
                std::memcpy(d1.get(), data1.get(), _size * sizeof(T));
                std::memcpy(d2.get(), data2.get(), _size * sizeof(T));

                MemoryOperations::swap<T, ExecutionTraitSIMD<T_SIMD::AVX512, true, T_SIMDStore::Cached, DefaultUnroll()>>(_size, d1.get(), d2.get());

                TestUtility::compareVectors<T>(d1.get(), expected_in_data1.get(), _size, name + " d1");
                TestUtility::compareVectors<T>(d2.get(), expected_in_data2.get(), _size, name + " d2");
            }

            {
                name = {"SIMD_AVX512_Unaligned_Cached"};
                auto d1 = createAlignedUniquePtr<T>(_size);
                auto d2 = createAlignedUniquePtr<T>(_size);
                std::memcpy(d1.get(), data1.get(), _size * sizeof(T));
                std::memcpy(d2.get(), data2.get(), _size * sizeof(T));

                MemoryOperations::swap<T, ExecutionTraitSIMD<T_SIMD::AVX512, false, T_SIMDStore::Cached, DefaultUnroll()>>(_size, d1.get(), d2.get());

                TestUtility::compareVectors<T>(d1.get(), expected_in_data1.get(), _size, name + " d1");
                TestUtility::compareVectors<T>(d2.get(), expected_in_data2.get(), _size, name + " d2");
            }

            {
                name = {"SIMD_AVX512_Aligned_Streamed"};
                auto d1 = createAlignedUniquePtr<T>(_size);
                auto d2 = createAlignedUniquePtr<T>(_size);
                std::memcpy(d1.get(), data1.get(), _size * sizeof(T));
                std::memcpy(d2.get(), data2.get(), _size * sizeof(T));

                MemoryOperations::swap<T, ExecutionTraitSIMD<T_SIMD::AVX512, true, T_SIMDStore::Streamed, DefaultUnroll()>>(_size, d1.get(), d2.get());

                TestUtility::compareVectors<T>(d1.get(), expected_in_data1.get(), _size, name + " d1");
                TestUtility::compareVectors<T>(d2.get(), expected_in_data2.get(), _size, name + " d2");
            }

            {
                name = {"SIMD_AVX512_Unaligned_Streamed"};
                auto d1 = createAlignedUniquePtr<T>(_size);
                auto d2 = createAlignedUniquePtr<T>(_size);
                std::memcpy(d1.get(), data1.get(), _size * sizeof(T));
                std::memcpy(d2.get(), data2.get(), _size * sizeof(T));

                MemoryOperations::swap<T, ExecutionTraitSIMD<T_SIMD::AVX512, false, T_SIMDStore::Streamed, DefaultUnroll()>>(_size, d1.get(), d2.get());

                TestUtility::compareVectors<T>(d1.get(), expected_in_data1.get(), _size, name + " d1");
                TestUtility::compareVectors<T>(d2.get(), expected_in_data2.get(), _size, name + " d2");
            }
            #endif

            #if defined(STALKER_THREADING_ENABLE) && STALKER_THREADING_ENABLE != 0

            ThreadTraitSTDThread threadTrait = ThreadTraitSTDThread();
            auto nThreads = threadTrait.getNumThreads();
            auto nThreadsStr = std::to_string(nThreads);

            {
                name = {"Parallel_Classic (Loop)"};
                auto d1 = createAlignedUniquePtr<T>(_size);
                auto d2 = createAlignedUniquePtr<T>(_size);
                std::memcpy(d1.get(), data1.get(), _size * sizeof(T));
                std::memcpy(d2.get(), data2.get(), _size * sizeof(T));

                constexpr bool IsSTD = false;
                MemoryOperations::swap<T, ThreadTraitSTDThread, ExecutionTraitScalar<IsSTD>>(threadTrait, _size, d1.get(), d2.get());

                TestUtility::compareVectors<T>(d1.get(), expected_in_data1.get(), _size, name + " d1");
                TestUtility::compareVectors<T>(d2.get(), expected_in_data2.get(), _size, name + " d2");
            }

            {
                name = {"Parallel_Classic (std::swap_ranges)"};
                auto d1 = createAlignedUniquePtr<T>(_size);
                auto d2 = createAlignedUniquePtr<T>(_size);
                std::memcpy(d1.get(), data1.get(), _size * sizeof(T));
                std::memcpy(d2.get(), data2.get(), _size * sizeof(T));
                
                constexpr bool IsSTD = true;
                MemoryOperations::swap<T, ThreadTraitSTDThread, ExecutionTraitScalar<IsSTD>>(threadTrait, _size, d1.get(), d2.get());

                TestUtility::compareVectors<T>(d1.get(), expected_in_data1.get(), _size, name + " d1");
                TestUtility::compareVectors<T>(d2.get(), expected_in_data2.get(), _size, name + " d2");
            }

            {
                name = {"Parallel_Meta"};
                auto d1 = createAlignedUniquePtr<T>(_size);
                auto d2 = createAlignedUniquePtr<T>(_size);
                std::memcpy(d1.get(), data1.get(), _size * sizeof(T));
                std::memcpy(d2.get(), data2.get(), _size * sizeof(T));

                MemoryOperations::swap<T, ThreadTraitSTDThread, ExecutionTraitUnrolled<>>(threadTrait, _size, d1.get(), d2.get());

                TestUtility::compareVectors<T>(d1.get(), expected_in_data1.get(), _size, name + " d1");
                TestUtility::compareVectors<T>(d2.get(), expected_in_data2.get(), _size, name + " d2");
            }
            
            #if defined(STALKER_SIMD_AVX2_OK)
            {
                name = {"Parallel_SIMD_AVX2_Aligned_Cached"};
                auto d1 = createAlignedUniquePtr<T>(_size);
                auto d2 = createAlignedUniquePtr<T>(_size);
                std::memcpy(d1.get(), data1.get(), _size * sizeof(T));
                std::memcpy(d2.get(), data2.get(), _size * sizeof(T));

                MemoryOperations::swap<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2, true, T_SIMDStore::Cached, DefaultUnroll()>>(threadTrait, _size, d1.get(), d2.get());

                TestUtility::compareVectors<T>(d1.get(), expected_in_data1.get(), _size, name + " d1");
                TestUtility::compareVectors<T>(d2.get(), expected_in_data2.get(), _size, name + " d2");
            }

            {
                name = {"Parallel_SIMD_AVX2_Unaligned_Cached"};
                auto d1 = createAlignedUniquePtr<T>(_size);
                auto d2 = createAlignedUniquePtr<T>(_size);
                std::memcpy(d1.get(), data1.get(), _size * sizeof(T));
                std::memcpy(d2.get(), data2.get(), _size * sizeof(T));

                MemoryOperations::swap<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2, false, T_SIMDStore::Cached, DefaultUnroll()>>(threadTrait, _size, d1.get(), d2.get());

                TestUtility::compareVectors<T>(d1.get(), expected_in_data1.get(), _size, name + " d1");
                TestUtility::compareVectors<T>(d2.get(), expected_in_data2.get(), _size, name + " d2");
            }

            {
                name = {"Parallel_SIMD_AVX2_Aligned_Streamed"};
                auto d1 = createAlignedUniquePtr<T>(_size);
                auto d2 = createAlignedUniquePtr<T>(_size);
                std::memcpy(d1.get(), data1.get(), _size * sizeof(T));
                std::memcpy(d2.get(), data2.get(), _size * sizeof(T));

                MemoryOperations::swap<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2, true, T_SIMDStore::Streamed, DefaultUnroll()>>(threadTrait, _size, d1.get(), d2.get());

                TestUtility::compareVectors<T>(d1.get(), expected_in_data1.get(), _size, name + " d1");
                TestUtility::compareVectors<T>(d2.get(), expected_in_data2.get(), _size, name + " d2");
            }

            {
                name = {"Parallel_SIMD_AVX2_Unaligned_Streamed"};
                auto d1 = createAlignedUniquePtr<T>(_size);
                auto d2 = createAlignedUniquePtr<T>(_size);
                std::memcpy(d1.get(), data1.get(), _size * sizeof(T));
                std::memcpy(d2.get(), data2.get(), _size * sizeof(T));

                MemoryOperations::swap<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2, false, T_SIMDStore::Streamed, DefaultUnroll()>>(threadTrait, _size, d1.get(), d2.get());

                TestUtility::compareVectors<T>(d1.get(), expected_in_data1.get(), _size, name + " d1");
                TestUtility::compareVectors<T>(d2.get(), expected_in_data2.get(), _size, name + " d2");
            }
            #endif

            #if defined(STALKER_SIMD_AVX512_OK)
            {
                name = {"Parallel_SIMD_AVX512_Aligned_Cached"};
                auto d1 = createAlignedUniquePtr<T>(_size);
                auto d2 = createAlignedUniquePtr<T>(_size);
                std::memcpy(d1.get(), data1.get(), _size * sizeof(T));
                std::memcpy(d2.get(), data2.get(), _size * sizeof(T));

                MemoryOperations::swap<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512, true, T_SIMDStore::Cached, DefaultUnroll()>>(threadTrait, _size, d1.get(), d2.get());

                TestUtility::compareVectors<T>(d1.get(), expected_in_data1.get(), _size, name + " d1");
                TestUtility::compareVectors<T>(d2.get(), expected_in_data2.get(), _size, name + " d2");
            }

            {
                name = {"Parallel_SIMD_AVX512_Unaligned_Cached"};
                auto d1 = createAlignedUniquePtr<T>(_size);
                auto d2 = createAlignedUniquePtr<T>(_size);
                std::memcpy(d1.get(), data1.get(), _size * sizeof(T));
                std::memcpy(d2.get(), data2.get(), _size * sizeof(T));

                MemoryOperations::swap<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512, false, T_SIMDStore::Cached, DefaultUnroll()>>(threadTrait, _size, d1.get(), d2.get());

                TestUtility::compareVectors<T>(d1.get(), expected_in_data1.get(), _size, name + " d1");
                TestUtility::compareVectors<T>(d2.get(), expected_in_data2.get(), _size, name + " d2");
            }

            {
                name = {"Parallel_SIMD_AVX512_Aligned_Streamed"};
                auto d1 = createAlignedUniquePtr<T>(_size);
                auto d2 = createAlignedUniquePtr<T>(_size);
                std::memcpy(d1.get(), data1.get(), _size * sizeof(T));
                std::memcpy(d2.get(), data2.get(), _size * sizeof(T));

                MemoryOperations::swap<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512, true, T_SIMDStore::Streamed, DefaultUnroll()>>(threadTrait, _size, d1.get(), d2.get());

                TestUtility::compareVectors<T>(d1.get(), expected_in_data1.get(), _size, name + " d1");
                TestUtility::compareVectors<T>(d2.get(), expected_in_data2.get(), _size, name + " d2");
            }

            {
                name = {"Parallel_SIMD_AVX512_Unaligned_Streamed"};
                auto d1 = createAlignedUniquePtr<T>(_size);
                auto d2 = createAlignedUniquePtr<T>(_size);
                std::memcpy(d1.get(), data1.get(), _size * sizeof(T));
                std::memcpy(d2.get(), data2.get(), _size * sizeof(T));

                MemoryOperations::swap<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512, false, T_SIMDStore::Streamed, DefaultUnroll()>>(threadTrait, _size, d1.get(), d2.get());

                TestUtility::compareVectors<T>(d1.get(), expected_in_data1.get(), _size, name + " d1");
                TestUtility::compareVectors<T>(d2.get(), expected_in_data2.get(), _size, name + " d2");
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
    };

} // namespace STLKR_Tests