//
// Created by hal9000 on 6/21/24.
//

#ifndef STALKER_THREADJOB_H
#define STALKER_THREADJOB_H

template<typename threadJob>
struct JobArgs {
    threadJob job;
    unsigned startIndex;
    unsigned endIndex;
    unsigned L0CacheSize;
};

template<typename threadJob, typename T>
struct ReducedJobArgs {
    threadJob job;
    unsigned startIndex;
    unsigned endIndex;
    T* result;
    unsigned L0CacheSize;
};

template<typename threadJob>
static void* jobWrapper(void* args) {
    auto jobArgs = static_cast<JobArgs<threadJob>*>(args);
    jobArgs->job(jobArgs->startIndex, jobArgs->endIndex, jobArgs->L0CacheSize);
    delete jobArgs;  // Clean up dynamically allocated memory
    return nullptr;
}

template<typename threadJob, typename T>
static void* reducedJobWrapper(void* args) {
    auto jobArgs = static_cast<ReducedJobArgs<threadJob, T>*>(args);
    jobArgs->job(jobArgs->startIndex, jobArgs->endIndex, jobArgs->result, jobArgs->L0CacheSize);
    delete jobArgs;  // Clean up dynamically allocated memory
    return nullptr;
}

#endif //STALKER_THREADJOB_H
