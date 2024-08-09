//
// Created by hal9000 on 6/19/24.
//

#ifndef STALKER_THREAD_OPERATIONS_H
#define STALKER_THREAD_OPERATIONS_H

#include <list>
#include <numeric>
#include "ThreadJob.h"
#include "../MachineTopology/CPUTopologyLinux.h"
#include "CPU_Manager.h"

enum class WorkDistributionPolicy{
    Slave_Stoker,
    Slave_Slave,
    Slave,
    Stoker,
    Master
};

class Thread_Operations {
public:



    template <typename threadJob>
    static inline void executeJob(threadJob job, unsigned size, unsigned numCores, bool enableHyperThreading, CPU_Manager &manager) {
        cpu_set_t thisJobCoreSet;
        CPU_ZERO(&thisJobCoreSet);
        auto cores = std::move(manager.getCores(numCores, true));
        auto threadPool = std::list<Thread*>();
        for (auto & core : cores) {
            core->addThreadsToPool(threadPool, enableHyperThreading);
            core->setThreadAffinity(thisJobCoreSet);
        }
        unsigned threadBlockSize = (size + threadPool.size() - 1) / threadPool.size();
        unsigned startIndex, endIndex, iThread, cacheSize;
        for (const auto &thread : threadPool) {
            startIndex = iThread * threadBlockSize;
            endIndex = std::min((iThread + 1) * threadBlockSize, size);
            cacheSize = thread->getSharedCacheMemory()->getCacheLevel1Data()->getSize();
            thread->executeJob(job, startIndex, endIndex, cacheSize);
            iThread++;
        }
        for (const auto &thread : threadPool) {
            thread->join();
            thread->resetThreadAffinity();
        }
        manager.release(std::move(cores));
    }

    template <typename threadJob>
    static inline void executeJob(threadJob job, unsigned numCores, bool enableHyperThreading,
                           std::pair<unsigned, unsigned>* threadRange, CPU_Manager &manager) {

        auto slaveThreadPool = std::list<Thread*>();
        auto cores = manager.getCores(numCores, enableHyperThreading);
        if (enableHyperThreading) {
            for (auto &core: cores){
                core->setThreadAffinity();
                core->addThreadsToPool(slaveThreadPool, enableHyperThreading);
            }
        }
        else{
            cpu_set_t thisJobCoreSet;
            CPU_ZERO(&thisJobCoreSet);
            for (auto & core : cores) {
                core->setThreadAffinity(thisJobCoreSet);
                core->addThreadsToPool(slaveThreadPool, enableHyperThreading);
            }
        }
        unsigned iThread = 0;
        for (const auto &thread : slaveThreadPool) {
            thread->executeJob(job, threadRange[iThread].first, threadRange[iThread].second ,
                               thread->getSharedCacheMemory()->getCacheLevel1Data()->getSize());
            iThread++;
        }
        for (const auto &thread : slaveThreadPool) {
            thread->join();
            thread->resetThreadAffinity();
        }
        manager.release(cores);
    }

    template <typename T, typename threadJob>
    static inline T executeJobWithReduction(threadJob job, unsigned size, unsigned numCores, bool enableHyperThreading,
                                  std::pair<unsigned, unsigned>* threadRange, CPU_Manager &manager) {

        auto slaveThreadPool = std::list<Thread*>();
        auto cores = manager.getCores(numCores, enableHyperThreading);
        if (enableHyperThreading) {
            for (auto &core: cores){
                core->setThreadAffinity();
                core->addThreadsToPool(slaveThreadPool, enableHyperThreading);
            }
        }
        else{
            cpu_set_t thisJobCoreSet;
            CPU_ZERO(&thisJobCoreSet);
            for (auto & core : cores) {
                core->setThreadAffinity(thisJobCoreSet);
                core->addThreadsToPool(slaveThreadPool, enableHyperThreading);
            }
        }
        int iThread = 0;
        auto reducedResult = std::vector<T>(numCores, 0);
        for (const auto &thread : slaveThreadPool){
            thread->executeJobWithReduction<threadJob, T>(job, threadRange[iThread].first, threadRange[iThread].second, &reducedResult[iThread], 
                                                           thread->getSharedCacheMemory()->getCacheLevel1Data()->getSize());
            iThread++;
        }
        for (const auto &thread : slaveThreadPool) {
            thread->join();
            thread->resetThreadAffinity();
        }
        manager.release(std::move(cores));
        return std::accumulate(reducedResult.begin(), reducedResult.end(), 0);
    }
    
private:

};


#endif //STALKER_STLKR_THREAD_OPERATIONSLINUX_HB
