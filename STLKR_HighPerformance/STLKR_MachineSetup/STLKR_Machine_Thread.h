//
// Created by hal9000 on 6/16/24.
//

#ifndef STALKER_STLKR_MACHINE_THREAD_H
#define STALKER_STLKR_MACHINE_THREAD_H

#include "STLKR_Machine_SharedCache.h"
#include <pthread.h>


class STLKR_Machine_Thread{
public:
    STLKR_Machine_Thread() = default;
    STLKR_Machine_Thread(unsigned id, unsigned clockMin, unsigned clockMax, STLKR_Machine_SharedCache *sharedCacheMemory);
    ~STLKR_Machine_Thread() = default;
    unsigned getId() const;
    unsigned getClockMin() const;
    unsigned getClockMax() const;
    bool isRunning() const;
    const STLKR_Machine_SharedCache *getSharedCacheMemory();
    void setThreadAffinity(cpu_set_t &coreSet, unsigned coreId);
    void resetThreadAffinity();
    void join() const;

    template<typename threadJob>
    void executeJob(threadJob job, unsigned startIndex, unsigned endIndex, cpu_set_t coreSet) {
        auto jobArgs = new JobArgs<threadJob>{job, startIndex, endIndex, coreSet};
        pthread_create(&_pThread, &_pThreadAttribute, _jobWrapper<threadJob>, jobArgs);
    }


private:

    template<typename threadJob>
    struct JobArgs {
        threadJob job;
        unsigned startIndex;
        unsigned endIndex;
        cpu_set_t coreSet;
    };

    template<typename threadJob>
    static void* _jobWrapper(void* args) {
        auto jobArgs = static_cast<JobArgs<threadJob>*>(args);
        pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &jobArgs->coreSet);  // Set thread affinity
        jobArgs->job(jobArgs->startIndex, jobArgs->endIndex);
        delete jobArgs;  // Clean up dynamically allocated memory
        return nullptr;
    }

    unsigned _id;
    unsigned _clockMin;
    unsigned _clockMax;
    bool _isRunning;
    STLKR_Machine_SharedCache *_sharedCacheMemory;
    pthread_t _pThread;
    pthread_attr_t _pThreadAttribute{};

    static inline void _initializeAttribute(pthread_attr_t& attribute);
    static inline void _destroyAttribute(pthread_attr_t& attribute);
};


#endif //STALKER_STLKR_MACHINE_THREAD_H
