//
// Created by hal9000 on 6/19/24.
//

#ifndef STALKER_THREAD_OPERATIONS_H
#define STALKER_THREAD_OPERATIONS_H

#include <list>
#include "JobArguments.h"
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
        auto cores = std::move(manager.getCores(numCores));
        auto threadPool = std::list<Thread*>();
        for (auto & core : cores) {
            core->setHyperThreading(enableHyperThreading);
            core->addThreadsToPool(threadPool);
            core->setThreadAffinity(thisJobCoreSet);
        }
        unsigned threadBlockSize = (size + threadPool.size() - 1) / threadPool.size();
        unsigned startIndex, endIndex, iThread;
        for (const auto &thread : threadPool) {
            startIndex = iThread * threadBlockSize;
            endIndex = std::min((iThread + 1) * threadBlockSize, size);
            thread->executeJob(job, startIndex, endIndex, thisJobCoreSet);
            iThread++;
        }
        for (const auto &thread : threadPool) {
            thread->join();
            thread->resetThreadAffinity();
        }
        manager.release(std::move(cores));
    }

    template <typename threadJob>
    static inline void executeJob(threadJob job, unsigned size, unsigned numCores, CPU_Manager &manager) {
        auto slaveThreadPool = std::list<Thread*>();
        auto stokerThreadPool = std::list<Thread*>();
        auto cores = std::move(manager.getCores(numCores));
        for (const auto &core : cores) {
            core->setThreadAffinity();
            core->addSlaveThreadsToPool(slaveThreadPool);
            core->addStokerThreadsToPool(stokerThreadPool);
        }
        unsigned threadBlockSize = (size + slaveThreadPool.size() - 1) / slaveThreadPool.size();
        unsigned startIndex, endIndex, iThread;
        for (const auto &thread : slaveThreadPool) {
            startIndex = iThread * threadBlockSize;
            endIndex = std::min((iThread + 1) * threadBlockSize, size);
            thread->executeJob(job, startIndex, endIndex, thread->getCoreSet());
            iThread++;
        }
        for (const auto &thread : slaveThreadPool) {
            thread->join();
            thread->resetThreadAffinity();
        }
        manager.release(std::move(cores));
    }

    template <typename threadJob>
    static inline void executeJob(threadJob job, unsigned size, unsigned numCores,
                           std::pair<unsigned, unsigned>* threadRange, CPU_Manager &manager) {

        auto slaveThreadPool = std::list<Thread*>();
        auto stokerThreadPool = std::list<Thread*>();
        auto cores = manager.getCores(numCores);
        for (const auto &core : cores) {
            core->addSlaveThreadsToPool(slaveThreadPool);
            core->addStokerThreadsToPool(stokerThreadPool);
            core->setThreadAffinity();
        }
        unsigned iThread = 0;
        for (const auto &thread : slaveThreadPool) {
            thread->executeJob(job, threadRange[iThread].first, threadRange[iThread].second, thread->getCoreSet());
            iThread++;
        }
        for (const auto &thread : slaveThreadPool) {
            thread->join();
            thread->resetThreadAffinity();
        }
        manager.release(std::move(cores));
    }
    
private:
};


#endif //STALKER_STLKR_THREAD_OPERATIONSLINUX_HB
