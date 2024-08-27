//
// Created by hal9000 on 6/16/24.
//

#ifndef STALKER_THREAD_H
#define STALKER_THREAD_H

#include "Cache.h"
#include "../ThreadJob.h"
#include <pthread.h>


class Thread{
public:
    Thread() = default;
    Thread(unsigned id, unsigned parentId, unsigned clockMin, unsigned clockMax, SharedCache *sharedCacheMemory) :
        _id(id), _parentId(parentId), _clockMin(clockMin), _clockMax(clockMax),
        _sharedCacheMemory(sharedCacheMemory), _pThread(0), _pThreadAttribute({}) { }


    ~Thread() = default;
    
    [[nodiscard]] inline unsigned getId() const { return _id; }
    
    [[nodiscard]] inline unsigned getParentId() const{ return _parentId; }
    
    [[nodiscard]] inline unsigned getClockMin() const { return _clockMin; }
    
    [[nodiscard]] inline unsigned getClockMax() const { return _clockMax; }
    
    const SharedCache *getSharedCache() { return _sharedCacheMemory; }
    
    void setThreadAffinity(const cpu_set_t &coreSet) {
        pthread_attr_init(&_pThreadAttribute);
        int result = pthread_attr_setaffinity_np(&_pThreadAttribute, sizeof(cpu_set_t), &coreSet);
        if (result != 0) std::cerr << "Error setting affinity for thread " << _id << std::endl;
    }
    
    void resetThreadAffinity(){
        pthread_attr_destroy(&_pThreadAttribute);
    }
    
    inline void addToCoreSet(cpu_set_t &coreSet) const {
        CPU_SET(_id, &coreSet);
    }
    
    void join() const {
        pthread_join(_pThread, nullptr);
    }

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
    SharedCache *_sharedCacheMemory;
    pthread_t _pThread;
    pthread_attr_t _pThreadAttribute;

    static inline void _initializeAttribute(pthread_attr_t& attribute);
    static inline void _destroyAttribute(pthread_attr_t& attribute);
};


#endif //STALKER_THREAD_H
