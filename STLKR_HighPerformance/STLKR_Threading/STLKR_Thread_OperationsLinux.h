//
// Created by hal9000 on 6/19/24.
//

#ifndef STALKER_STLKR_THREAD_OPERATIONSLINUX_H
#define STALKER_STLKR_THREAD_OPERATIONSLINUX_H
#include <pthread.h>
#include "../STLKR_MachineSetup/STLKR_Machine_Core.h"

class STLKR_Thread_OperationsLinux {
public:
    template<typename threadJob>
    static inline void executeJob(threadJob job, unsigned size, const std::vector<STLKR_Machine_Core *> &cores, bool enableHyperThreading = false) {
        unsigned availableThreads = 0;
        for (const auto &core : cores)
            availableThreads += core->getThreadCount(enableHyperThreading);
        availableThreads = std::min(availableThreads, size);
        unsigned blockSize = size / cores.size();

        for (int iPhysicalCore = 0; iPhysicalCore < cores.size(); ++iPhysicalCore) {
            auto startIndex = iPhysicalCore * blockSize;
            auto endIndex = (iPhysicalCore + 1 == cores.size() ? size : (iPhysicalCore + 1) * blockSize);
            cores[iPhysicalCore]->executeJob(job, startIndex, endIndex, enableHyperThreading);
        }
    }

};


#endif //STALKER_STLKR_THREAD_OPERATIONSLINUX_HB
