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

//
// Created by hal9000 on 6/19/24.
//

#pragma once

#include <cmath>
#include <vector>
#include <atomic>
#include <Stalker/Threading/ThreadingTraits.h>

namespace Stalker::Threading {

    enum class T_ThreadOperation {
        Unary,
        UnaryReduced,
        Binary,
        BinaryReduced,
        BinaryMutable,
    };

    template <typename T, typename ThreadTrait, T_ThreadOperation OpT, typename Child>
    struct LauncherBase {

        template<typename JobT, typename... Args>
        inline static auto call(const ThreadTrait& trait, JobT&& job, size_t size, Args&&... args) {
            if (size == 0) {
                if constexpr (_isReduced()) return T{};
                else return;
            }

            const size_t numThreads = trait.getNumThreads();
            const size_t loopBlockSize = trait.getLoopBlockSize();
            size_t blockSize = (size + numThreads - 1) / numThreads;
            if (loopBlockSize > 1) {
                size_t rem = blockSize % loopBlockSize;
                if (rem != 0) blockSize += (loopBlockSize - rem);
            }

            const size_t effectiveThreads = (size + blockSize - 1) / blockSize;
            
            std::vector<PlatformThread> threads(effectiveThreads);
            std::vector<T> reducedResult(_isReduced() ? effectiveThreads : 1);

            for (size_t iThread = 0; iThread < effectiveThreads; ++iThread) {
                threads[iThread] = PlatformThread([&, iThread]() {
                    size_t start = iThread * blockSize;
                    size_t end = std::min(start + blockSize, size);
                    if (start < end) {
                        if constexpr (!_isReduced())
                            Child::_call(job, end - start, start, std::forward<Args>(args)...);
                        else
                            reducedResult[iThread] = Child::_call(job, end - start, start, std::forward<Args>(args)...);
                    }
                });
            }
            for (size_t i = 0; i < effectiveThreads; ++i)
                if (threads[i].joinable()) threads[i].join();

            if constexpr (_isReduced()) {
                T finalResult = reducedResult[0];
                for (size_t i = 1; i < effectiveThreads; ++i) {
                    finalResult += reducedResult[i];
                }
                return finalResult;
            }
        }

    private:
        
        constexpr static bool _isReduced() {
            return OpT == T_ThreadOperation::UnaryReduced || OpT == T_ThreadOperation::BinaryReduced;
        }
    };

    template<typename T, typename ThreadTrait, T_ThreadOperation OpT> struct Launcher;

    template<typename T, typename ThreadTrait>
    struct Launcher<T, ThreadTrait, T_ThreadOperation::Binary>
        : public LauncherBase<T, ThreadTrait, T_ThreadOperation::Binary, Launcher<T, ThreadTrait, T_ThreadOperation::Binary>> {
        
        using Base = LauncherBase<T, ThreadTrait, T_ThreadOperation::Binary, Launcher<T, ThreadTrait, T_ThreadOperation::Binary>>;

    protected:

        friend Base;

        template<typename JobT, typename... Args>
        inline static void _call(JobT&& job, size_t size, size_t start, const T* a, const T* b, T* result, Args&&... args) {
            job.call(size, a + start, b + start, result + start, std::forward<Args>(args)...);
        }

        template<typename JobT, typename... Args>
        inline static void _call(JobT&& job, size_t size, size_t start, T* a, const T* b, Args&&... args) {
            job.call(size, a + start, b + start, std::forward<Args>(args)...);
        }
    };

    template<typename T, typename ThreadTrait>
    struct Launcher<T, ThreadTrait, T_ThreadOperation::BinaryMutable>
        : public LauncherBase<T, ThreadTrait, T_ThreadOperation::BinaryMutable, Launcher<T, ThreadTrait, T_ThreadOperation::BinaryMutable>> {
        
        using Base = LauncherBase<T, ThreadTrait, T_ThreadOperation::BinaryMutable, Launcher<T, ThreadTrait, T_ThreadOperation::BinaryMutable>>;

    protected:

        friend Base;

        template<typename JobT, typename... Args>
        inline static void _call(JobT&& job, size_t size, size_t start, T* a, T* b, Args&&... args) {
            job.call(size, a + start, b + start, std::forward<Args>(args)...);
        }
    };

    template<typename T, typename ThreadTrait>
    struct Launcher<T, ThreadTrait, T_ThreadOperation::Unary>
        : public LauncherBase<T, ThreadTrait, T_ThreadOperation::Unary, Launcher<T, ThreadTrait, T_ThreadOperation::Unary>> {

        using Base = LauncherBase<T, ThreadTrait, T_ThreadOperation::Unary, Launcher<T, ThreadTrait, T_ThreadOperation::Unary>>;

    protected:

        friend Base;

        template<typename JobT, typename... Args>
        inline static void _call(JobT&& job, size_t size, size_t start, const T* a, T* result, Args&&... args) {
            job.call(size, a + start, result + start, std::forward<Args>(args)...);
        }

        template<typename JobT, typename... Args>
        inline static void _call(JobT&& job, size_t size, size_t start, T* a, Args&&... args) {
            job.call(size, a + start, std::forward<Args>(args)...);
        }
    };

    template<typename T, typename ThreadTrait>
    struct Launcher<T, ThreadTrait, T_ThreadOperation::BinaryReduced>
        : public LauncherBase<T, ThreadTrait, T_ThreadOperation::BinaryReduced, Launcher<T, ThreadTrait, T_ThreadOperation::BinaryReduced>> {

        using Base = LauncherBase<T, ThreadTrait, T_ThreadOperation::BinaryReduced, Launcher<T, ThreadTrait, T_ThreadOperation::BinaryReduced>>;

    protected:

        friend Base;

        template<typename JobT, typename... Args>
        inline static T _call(JobT&& job, size_t size, size_t start, const T* a, const T* b, Args&&... args) {
            return job.call(size, a + start, b + start, std::forward<Args>(args)...);
        }
    };

    template<typename T, typename ThreadTrait>
    struct Launcher<T, ThreadTrait, T_ThreadOperation::UnaryReduced>
        : public LauncherBase<T, ThreadTrait, T_ThreadOperation::UnaryReduced, Launcher<T, ThreadTrait, T_ThreadOperation::UnaryReduced>> {
        using Base = LauncherBase<T, ThreadTrait, T_ThreadOperation::UnaryReduced, Launcher<T, ThreadTrait, T_ThreadOperation::UnaryReduced>>;

    protected:
    
        friend Base;

        template<typename JobT, typename... Args>
        inline static T _call(JobT&& job, size_t size, size_t start, const T* a, Args&&... args) {
            return job.call(size, a + start, std::forward<Args>(args)...);
        }
    };
} // namespace Stalker::Threading