//
// Created by hal9000 on 6/19/24.
//

#pragma once

#include <cmath>
#include <vector>
#include <Stalker/Threading/ThreadingTraits.h>

namespace Stalker::Threading {

    enum class T_ThreadOperation {
        Unary,
        UnaryReduced,
        Binary,
        BinaryReduced,
    };

    template <typename T, typename ThreadTrait, T_ThreadOperation OpT, typename Child>
    struct LauncherBase {

        template<typename JobT, typename... Args>
        inline static auto call(const ThreadTrait& trait, JobT&& job, size_t size, Args&&... args) {
            const size_t numThreads = trait.getNumThreads();
            const size_t blockSize  = (size + numThreads - 1) / numThreads;
            PlatformThread threads[numThreads];
            T reducedResult[_isReduced() ? numThreads : 1];


            for (size_t iThread = 0; iThread < numThreads; ++iThread) {
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
            for (auto& thread : threads)
                if (thread.joinable()) thread.join();
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