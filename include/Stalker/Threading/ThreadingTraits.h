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
            if (numThreads == 0)
                numThreads = 1;
            else if (numThreads > Child::_maxNumThreads())
                _numThreads = Child::_maxNumThreads();
            else
                _numThreads = numThreads;
        }

        inline size_t getLoopBlockSize() const {
            return _loopBlockSize;
        }

        inline void setLoopBlockSize(size_t loopBlockSize) {
            if (loopBlockSize == 0)
                _loopBlockSize = 1;
            else
                _loopBlockSize = loopBlockSize;
        }
        
    protected:

        ThreadTraitBase(size_t numThreads) {
            if (numThreads == 0)
                numThreads = 1;
            else if (numThreads > Child::_maxNumThreads())
                _numThreads = Child::_maxNumThreads();
            else
                _numThreads = numThreads;
            _loopBlockSize = 1;
        }

        size_t _numThreads;
        size_t _loopBlockSize;
    };
    
    #if defined(STALKER_THREADING_POSIX_ENABLE) && STALKER_THREADING_POSIX_ENABLE != 0
    struct ThreadTraitPThread : public ThreadTraitBase<ThreadType::PThread, ThreadTraitPThread> {
        using Base = ThreadTraitBase<ThreadType::PThread, ThreadTraitPThread>;

        ThreadTraitPThread(size_t numThreads = DefaultNumThreads() = 1) : Base(numThreads) {}

        protected:

        friend Base;
        
        static constexpr ThreadType _Type = ThreadType::PThread;

        static size_t _maxNumThreads() {
            return 2;
        }
    };
    #endif

    #if defined(STALKER_THREADING_STD_ENABLE) && STALKER_THREADING_STD_ENABLE != 0
    struct ThreadTraitSTDThread : public ThreadTraitBase<ThreadType::STDThread, ThreadTraitSTDThread> {
        using Base = ThreadTraitBase<ThreadType::STDThread, ThreadTraitSTDThread>;
        
        explicit ThreadTraitSTDThread(size_t numThreads = DefaultNumThreads()) : Base(numThreads) {}
        
        protected:

        friend Base;
        
        static constexpr ThreadType _Type = ThreadType::STDThread;

        static size_t _maxNumThreads() {
            return std::thread::hardware_concurrency();
        }
    };
    #endif
} // namespace Stalker::Threading
