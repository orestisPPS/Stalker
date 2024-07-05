//
// Created by hal9000 on 6/16/24.
//

#ifndef STALKER_THREAD_H
#define STALKER_THREAD_H

#include "SharedCache.h"
#include <pthread.h>


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
    void setThreadAffinity(cpu_set_t &coreSet);
    void resetThreadAffinity();
    cpu_set_t getCoreSet() const;
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
    unsigned _parentId;
    unsigned _clockMin;
    unsigned _clockMax;
    bool _isRunning;
    SharedCache *_sharedCacheMemory;
    pthread_t _pThread;
    pthread_attr_t _pThreadAttribute{};
    cpu_set_t _coreSet{};

    static inline void _initializeAttribute(pthread_attr_t& attribute);
    static inline void _destroyAttribute(pthread_attr_t& attribute);
};


#endif //STALKER_THREAD_H
