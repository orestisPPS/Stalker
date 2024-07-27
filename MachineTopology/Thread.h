//
// Created by hal9000 on 6/16/24.
//

#ifndef STALKER_THREAD_H
#define STALKER_THREAD_H

#include "SharedCache.h"
#include <pthread.h>
#include "../Operations/ThreadJob.h"


class Thread{
public:
    Thread() = default;
    Thread(unsigned id, unsigned parentId, unsigned clockMin, unsigned clockMax, SharedCache *sharedCacheMemory);


    ~Thread() = default;
    unsigned getId() const;
    unsigned getParentId() const;
    unsigned getClockMin() const;
    unsigned getClockMax() const;
    bool isRunning() const;
    const SharedCache *getSharedCacheMemory();
    void setThreadAffinity(const cpu_set_t &coreSet);
    void resetThreadAffinity();
    cpu_set_t & getCoreSet();
    void join() const;

    template<typename threadJob>
    void executeJob(threadJob job, unsigned startIndex, unsigned endIndex, unsigned L0CacheSize = 0) {
        auto jobArgs = new JobArgs<threadJob>{job, startIndex, endIndex, L0CacheSize};
        pthread_create(&_pThread, &_pThreadAttribute, jobWrapper<threadJob>, jobArgs);
    }

    template<typename threadJob, typename T>
    void executeJobWithReduction(threadJob job, unsigned startIndex, unsigned endIndex, T *result, unsigned L0CacheSize = 0) {
        auto jobArgs = new ReducedJobArgs<threadJob, T>{job, startIndex, endIndex, result, L0CacheSize};
        pthread_create(&_pThread, &_pThreadAttribute, reducedJobWrapper<threadJob, T>, jobArgs);
    }


private:

    unsigned _id;
    unsigned _parentId;
    unsigned _clockMin;
    unsigned _clockMax;
    bool _isRunning;
    SharedCache *_sharedCacheMemory;
    pthread_t _pThread;
    pthread_attr_t _pThreadAttribute;
    cpu_set_t _coreSet{};

    static inline void _initializeAttribute(pthread_attr_t& attribute);
    static inline void _destroyAttribute(pthread_attr_t& attribute);
};


#endif //STALKER_THREAD_H
