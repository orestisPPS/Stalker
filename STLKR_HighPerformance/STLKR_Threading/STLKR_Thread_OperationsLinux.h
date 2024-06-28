//
// Created by hal9000 on 6/19/24.
//

#ifndef STALKER_STLKR_THREAD_OPERATIONSLINUX_H
#define STALKER_STLKR_THREAD_OPERATIONSLINUX_H

#include <list>
#include "../STLKR_MachineSetup/STLKR_Machine_Core.h"
#include "STLKR_Threading_JobArguments.h"
class STLKR_Thread_OperationsLinux {
public:

    template <typename threadJob>
    static inline void executeJob(threadJob job, unsigned size, std::vector<STLKR_Machine_Core *> &cores, bool enableHyperThreading = false) {
        
        auto threadPool = std::vector<STLKR_Machine_Thread*>();
        cpu_set_t thisJobCoreSet;
        CPU_ZERO(&thisJobCoreSet);
        for (const auto &core : cores) {
            core->setHyperThreading(enableHyperThreading);
            auto coreThreads = core->getThreads();
            for (const auto &thread : coreThreads) {
                threadPool.push_back(thread);
                thread->setThreadAffinity(thisJobCoreSet, thread->getId());
            }
        }
        unsigned threadBlockSize = (size + threadPool.size() - 1) / threadPool.size();
        unsigned startIndex, endIndex;
        for (int iThread = 0; iThread < threadPool.size(); ++iThread) {
            startIndex = iThread * threadBlockSize;
            endIndex = std::min((iThread + 1) * threadBlockSize, size);
            //std::cout << "Core: " << cores[iThread]->getId() << " Start: " << startIndex << " End: " << endIndex << std::endl;
            threadPool[iThread]->executeJob(job, startIndex, endIndex, thisJobCoreSet);
        }
        for (const auto &thread : threadPool) {
            thread->join();
            thread->resetThreadAffinity();
        }
        
    }
    
private:
    static std::list<STLKR_Machine_Thread*> _getThreadPool(const std::vector<STLKR_Machine_Core *> &cores) {
        std::list<STLKR_Machine_Thread*> threadPool;
        for (const auto &core : cores) {
            auto coreThreads = core->getThreads();
            for (const auto &thread : coreThreads) {
                threadPool.push_back(thread);
            }
        }
        return threadPool;
    }

};


#endif //STALKER_STLKR_THREAD_OPERATIONSLINUX_HB
