//
// Created by hal9000 on 6/15/24.
//

#include <iostream>
#include <cstring>
#include "STLKR_Machine_Core.h"

STLKR_Machine_Core::STLKR_Machine_Core(unsigned id, std::vector<STLKR_Machine_Thread*> threads) {
    _id = id;
    _threads = std::move(threads);
    _pThreads = std::vector<pthread_t>(_threads.size());
    _pThreadAttributes = std::vector<pthread_attr_t>(_threads.size());
    _initializeThreadAttributes();
}

unsigned STLKR_Machine_Core::getId() const{
    return _id;
}

std::vector<STLKR_Machine_Thread *> STLKR_Machine_Core::getThreads(bool hyperThreadEnabled) const{
    if (hyperThreadEnabled)
        return _threads;
    else
        return std::vector<STLKR_Machine_Thread*>{_threads[0]};
}

unsigned STLKR_Machine_Core::getThreadCount(bool hyperThreadEnabled) const{
    if (hyperThreadEnabled)
        return _threads.size();
    else
        return 1;
}


void STLKR_Machine_Core::checkThreadAffinity() {
    for (int i = 0; i < _pThreads.size(); ++i) {
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);

        int result = pthread_getaffinity_np(_pThreads[i], sizeof(cpu_set_t), &cpuset);
        if (result != 0) {
            std::cerr << "Error getting affinity for thread " << i << ": " << strerror(result) << std::endl;
            continue;
        }

        std::cout << "Thread " << i << " CPU Affinity: ";
        for (int j = 0; j < CPU_SETSIZE; j++) {
            if (CPU_ISSET(j, &cpuset)) {
                std::cout << j << " ";
            }
        }
        std::cout << std::endl;
    }
}

void inline STLKR_Machine_Core::_initializeThreadAttributes() {
    for (auto & _pThreadAttribute : _pThreadAttributes) {
        pthread_attr_init(&_pThreadAttribute);
    }
}

void STLKR_Machine_Core::setThreadAffinity() {
    CPU_ZERO(&_thisCoreSet);
    CPU_SET(_id, &_thisCoreSet); // Set the CPU ID for the core
    for (int i = 0; i < _threads.size(); ++i) {
        int result = pthread_attr_setaffinity_np(&_pThreadAttributes[i], sizeof(cpu_set_t), &_thisCoreSet);
        if (result != 0) {
            std::cerr << "Error setting affinity for thread " << i << " to core " << _id << std::endl;
        }
    }
}

void STLKR_Machine_Core::resetThreadAffinity() {
    CPU_ZERO(&_thisCoreSet); // Reset the CPU set to an empty set
    for (size_t i = 0; i < _threads.size(); ++i) {
        int result = pthread_attr_setaffinity_np(&_pThreadAttributes[i], sizeof(cpu_set_t), &_thisCoreSet);
        if (result != 0) {
            std::cerr << "Error resetting affinity for thread " << i << " on core " << _id << std::endl;
        }
    }
}

template<typename threadJob>
void STLKR_Machine_Core::executeJob(threadJob job, unsigned startIndex, unsigned endIndex, bool hyperThreadEnabled) {
    auto threadsArguments = std::vector<JobArgs<threadJob>>(_threads.size());
    auto threads = getThreads(hyperThreadEnabled);
    for (int i = 0; i < threads.size(); ++i) {
        threadsArguments[i] = {job, startIndex, endIndex, threads[i]};
        pthread_create(&_pThreads[i], &_pThreadAttributes[i], threadFunctionWrapper<threadJob>, &threadsArguments[i]);
    }
}

template<typename threadJob>
void* threadFunctionWrapper(void* args) {
    auto jobArgs = static_cast<STLKR_Machine_Core::JobArgs<threadJob>*>(args);
    jobArgs->job(jobArgs->startIndex, jobArgs->endIndex, jobArgs->thread);
    return nullptr;
}


