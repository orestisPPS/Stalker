#pragma once

#include <cstddef>
#include <Stalker/Core/Config/Parallel.h>

namespace Stalker::Threading
{
    using namespace Stalker::Core::Config;

    // Specialization for STDThread
    template <ThreadType ThreadT, typename Child>
    struct ThreadTraitBase {

    public:

        static constexpr ThreadType Type = Child::_Type;

        static const size_t maxNumThreads() {
            return Child::_maxNumThreads();
        }

        inline size_t getNumThreads() const {
            return _numThreads;
        }

        inline void setNumThreads(size_t numThreads) {
            if (numThreads == 0 || numThreads > Child::_maxNumThreads())
                _numThreads = Child::_maxNumThreads();
            else
                _numThreads = numThreads;
        }

        inline size_t getLoopBlockSize() const {
            return _loopBlockSize;
        }

        inline void setLoopBlockSize(size_t loopBlockSize) {
            _loopBlockSize = loopBlockSize;
        }
        
    protected:
        explicit ThreadTraitBase(size_t numThreads)
            : _numThreads((numThreads == 0 || numThreads > Child::_maxNumThreads()) ? Child::_maxNumThreads() : numThreads) {
        }

        size_t _numThreads;

        size_t _loopBlockSize;
    };
    
    #if defined(STALKER_THREADING_POSIX_ENABLE) && STALKER_THREADING_POSIX_ENABLE != 0
    struct ThreadTraitPThread : public ThreadTraitBase<ThreadType::PThread, ThreadTraitPThread> {
        using Base = ThreadTraitBase<ThreadType::PThread, ThreadTraitPThread>;

        explicit ThreadTraitPThread(size_t numThreads = DefaultNumThreads()) : Base(numThreads) {}

        static constexpr ThreadType _Type = ThreadType::PThread;

    protected:
        friend Base;

        static size_t _maxNumThreads() {
            return 2;
        }

    };
    #endif

    #if defined(STALKER_THREADING_STD_ENABLE) && STALKER_THREADING_STD_ENABLE != 0
    struct ThreadTraitSTDThread : public ThreadTraitBase<ThreadType::STDThread, ThreadTraitSTDThread> {
        using Base = ThreadTraitBase<ThreadType::STDThread, ThreadTraitSTDThread>;
        
        explicit ThreadTraitSTDThread(size_t numThreads = DefaultNumThreads()) : Base(numThreads) {}
        
        static constexpr ThreadType _Type = ThreadType::STDThread;
    
    protected:
        friend Base;

        static size_t _maxNumThreads() {
            return std::thread::hardware_concurrency();
        }
    };
    #endif

    struct ThreadTraitNone : public ThreadTraitBase<ThreadType::None, ThreadTraitNone> {
        using Base = ThreadTraitBase<ThreadType::None, ThreadTraitNone>;
        using ThreadT = void; // No threading support
        explicit ThreadTraitNone(size_t numThreads = DefaultNumThreads()) : Base(numThreads) {}
        static constexpr ThreadType _Type = ThreadType::None;
    protected:
        friend Base;
        static size_t _maxNumThreads() {
            return 1;
        }
    };  

    
} // namespace Stalker::Threading
