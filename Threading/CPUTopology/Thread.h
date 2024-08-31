//
// Created by hal9000 on 6/16/24.
//

#ifndef STALKER_THREAD_H
#define STALKER_THREAD_H

#include "Cache.h"
#include "../ThreadJob.h"
#include <pthread.h>
#include <cassert>


class Thread{
public:
    Thread() = default;
    Thread(unsigned id, unsigned parentId, unsigned clockMin, unsigned clockMax, SharedCache *sharedCacheMemory) :
        _id(id), _parentId(parentId), _clockMin(clockMin), _clockMax(clockMax),
        _sharedCacheMemory(sharedCacheMemory), _pThread(0), _pThreadAttribute({}), _cpuSet(nullptr) { }


    ~Thread() = default;
    
    [[nodiscard]] inline unsigned getId() const { return _id; }
    
    [[nodiscard]] inline unsigned getParentId() const{ return _parentId; }
    
    [[nodiscard]] inline unsigned getClockMin() const { return _clockMin; }
    
    [[nodiscard]] inline unsigned getClockMax() const { return _clockMax; }
    
    const SharedCache *getSharedCache() { return _sharedCacheMemory; }
    
    inline void addToCoreSet(cpu_set_t *coreSet){
        _cpuSet = coreSet;
        CPU_SET(_id, _cpuSet);
    }
    
    void join() const {
        pthread_join(_pThread, nullptr);
    }

    template<typename threadJob>
    void executeJob(threadJob job, unsigned startIndex, unsigned endIndex, unsigned L0CacheSize = 0) {
        auto jobArgs = new JobArgs<threadJob>{job, startIndex, endIndex, L0CacheSize};
        pthread_create(&_pThread, nullptr, jobWrapper<threadJob>, jobArgs);
        assert(pthread_setaffinity_np(_pThread, sizeof(cpu_set_t), _cpuSet) == 0);
    }

    template<typename threadJob, typename T>
    void executeJobWithReduction(threadJob job, unsigned startIndex, unsigned endIndex, T *result, unsigned L0CacheSize = 0) {
        auto jobArgs = new ReducedJobArgs<threadJob, T>{job, startIndex, endIndex, result, L0CacheSize};
        pthread_create(&_pThread, nullptr, reducedJobWrapper<threadJob, T>, jobArgs);
        assert(pthread_setaffinity_np(_pThread, sizeof(cpu_set_t), _cpuSet) == 0);
    }


private:

    unsigned _id;
    unsigned _parentId;
    unsigned _clockMin;
    unsigned _clockMax;
    SharedCache *_sharedCacheMemory;
    pthread_t _pThread;
    pthread_attr_t _pThreadAttribute;
    cpu_set_t *_cpuSet;
};


#endif //STALKER_THREAD_H
