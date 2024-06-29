//
// Created by hal9000 on 6/19/24.
//

#ifndef STALKER_STLKR_THREAD_OPERATIONSLINUX_H
#define STALKER_STLKR_THREAD_OPERATIONSLINUX_H

#include "../STLKR_MachineSetup/STLKR_Machine_Core.h"
#include "STLKR_Threading_JobArguments.h"
class STLKR_Thread_OperationsLinux {
public:
    
    enum class WorkDistributionPolicy{
        SlaveAndStoker,
        SlaveAndMaster,
        Slave,
        Stoker,
        Master
    };
    

    template <typename threadJob>
    static inline void executeJob(threadJob job, unsigned size, std::vector<STLKR_Machine_Core *> &cores, bool enableHyperThreading = false) {
        cpu_set_t thisJobCoreSet;
        CPU_ZERO(&thisJobCoreSet);
        auto threadPool = std::vector<STLKR_Machine_Thread*>();
        for (const auto &core : cores) {
            core->setHyperThreading(enableHyperThreading);
            auto coreThreads = core->getThreads();
            for (const auto &thread : coreThreads) {
                threadPool.push_back(thread);
                thread->setThreadAffinity(thisJobCoreSet);
            }
        }
        unsigned threadBlockSize = (size + threadPool.size() - 1) / threadPool.size();
        unsigned startIndex, endIndex;
        for (int iThread = 0; iThread < threadPool.size(); ++iThread) {
            startIndex = iThread * threadBlockSize;
            endIndex = std::min((iThread + 1) * threadBlockSize, size);
            threadPool[iThread]->executeJob(job, startIndex, endIndex, thisJobCoreSet);
        }
        for (const auto &thread : threadPool) {
            thread->join();
            thread->resetThreadAffinity();
        }
    }

    template <typename threadJob>
    static inline void executeJobSIMD(threadJob job, unsigned size, std::vector<STLKR_Machine_Core *> &cores, std::pair<unsigned, unsigned>* threadRange) {
        cpu_set_t slaveCoreSet, stokerCoreSet;
        CPU_ZERO(&slaveCoreSet);
        CPU_ZERO(&stokerCoreSet);
        auto slaveThreadPool = std::vector<STLKR_Machine_Thread*>();
        auto stokerThreadPool = std::vector<STLKR_Machine_Thread*>();
        for (const auto &core : cores) {
            auto coreSlaves = core->getSlaveThreads();
            for (const auto &thread : coreSlaves) {
                slaveThreadPool.push_back(thread);
                thread->setThreadAffinity(slaveCoreSet);
            }
            auto coreStokers = core->getStokerThreads();
            for (const auto &thread : coreStokers) {
                stokerThreadPool.push_back(thread);
                thread->setThreadAffinity(stokerCoreSet);
            }
        }
        for (int iThread = 0; iThread < slaveThreadPool.size(); ++iThread) {
            slaveThreadPool[iThread]->executeJob(job, threadRange[iThread].first, threadRange[iThread].second, slaveCoreSet);
        }
        for (const auto &thread : slaveThreadPool) {
            thread->join();
            thread->resetThreadAffinity();
        }
    }
    
private:

};


#endif //STALKER_STLKR_THREAD_OPERATIONSLINUX_HB
