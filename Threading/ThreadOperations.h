//
// Created by hal9000 on 6/19/24.
//

#ifndef STALKER_THREAD_OPERATIONS_H
#define STALKER_THREAD_OPERATIONS_H

#include <list>
#include <numeric>
#include "../Threading/ThreadJob.h"
#include "../MachineTopology/CPUTopologyLinux.h"
#include "../MachineTopology/CPU_Manager.h"

class Thread_Operations {
public:

    template <typename threadJob>
    static inline void executeJob(threadJob job, unsigned size, unsigned blockSize, CPU_Manager &manager) {
        auto cores = manager.getCores();
        auto threadPool = _initializeThreadPool(cores, manager.isHyperthreadingEnabled());
        auto threadLimits = _getThreadsRange(size, threadPool.size(), blockSize);
        unsigned iThread = 0;
        for (const auto &thread : threadPool) {
            //std::cout<<thread->getId()<<" start: " << threadLimits[iThread].first << " end: " << threadLimits[iThread].second << std::endl; 
            thread->executeJob(job, threadLimits[iThread].first, threadLimits[iThread].second ,
                               thread->getSharedCacheMemory()->getCacheLevel(L1_Data)->getSize());
            iThread++;
        }
        for (const auto &thread : threadPool) {
            thread->join();
            thread->resetThreadAffinity();
        }
        manager.release(cores);
        delete[] threadLimits;
    }

    template <typename T, typename threadJob>
    static inline T executeJobWithReduction(threadJob job, unsigned size, unsigned blockSize, CPU_Manager &manager) {
        auto cores = manager.getCores();
        auto threadPool = _initializeThreadPool(cores, manager.isHyperthreadingEnabled());
        auto threadLimits = _getThreadsRange(size, threadPool.size(), blockSize);
        unsigned iThread = 0;
        auto reducedResult = std::vector<T>(threadPool.size(), 0);
        for (const auto &thread : threadPool){
            thread->executeJobWithReduction<threadJob, T>(job, threadLimits[iThread].first, threadLimits[iThread].second, &reducedResult[iThread], 
                                                           thread->getSharedCacheMemory()->getCacheLevel(L1_Data)->getSize());
            iThread++;
        }
        for (const auto &thread : threadPool) {
            thread->join();
            thread->resetThreadAffinity();
        }
        manager.release(cores);
        delete[] threadLimits;
        return std::accumulate(reducedResult.begin(), reducedResult.end(), 0);
    }
    
private:
    static std::list<Thread*> _initializeThreadPool(const std::vector<Core*> &cores, bool enableHyperThreading){
        auto threadPool = std::list<Thread*>();
        if (enableHyperThreading) {
            for (auto &core: cores){
                core->setThreadAffinity();
                core->addThreadsToPool(threadPool, enableHyperThreading);
            }
        }
        else{
            cpu_set_t thisJobCoreSet;
            CPU_ZERO(&thisJobCoreSet);
            for (auto & core : cores) {
                core->setThreadAffinity(thisJobCoreSet);
                core->addThreadsToPool(threadPool, enableHyperThreading);
            }
        }
        return threadPool;
    }

    static inline std::pair<unsigned, unsigned>* _getThreadsRange(unsigned size, unsigned numCores, unsigned blockSize){
        auto threadLimits = new std::pair<unsigned, unsigned>[numCores];
        unsigned totalBlocks = (size + blockSize - 1) / blockSize;
        unsigned threadBlockSize = (totalBlocks + numCores - 1) / numCores;
        unsigned startBlock = 0, endBlock = 0;
        for (unsigned i = 0; i < numCores; i++) {
            startBlock = i * threadBlockSize;
            endBlock = std::min(startBlock + threadBlockSize, totalBlocks);
            threadLimits[i].first = startBlock * blockSize;
            threadLimits[i].second = std::min(endBlock * blockSize, size);
        }
        return threadLimits;
    }
    
};


#endif //STALKER_STLKR_THREAD_OPERATIONSLINUX_HB
