//
// Created by hal9000 on 6/19/24.
//

#ifndef STALKER_STLKR_THREAD_OPERATIONSLINUX_H
#define STALKER_STLKR_THREAD_OPERATIONSLINUX_H
#include "../STLKR_MachineSetup/STLKR_Machine_Core.h"
#include "STLKR_Threading_JobArguments.h"
class STLKR_Thread_OperationsLinux {
public:


    template <typename threadJob>
    static inline void executeJob(threadJob job, unsigned size, std::vector<STLKR_Machine_Core *> &cores, bool enableHyperThreading = false) {

        unsigned coreBlockSize = size / cores.size();

        for (int i = 0; i < cores.size(); ++i) {
            cores[i]->setHyperThreading(enableHyperThreading);
            //cores[i]->setThreadAffinity();
            unsigned startIndex = i * coreBlockSize;
            unsigned endIndex = (i == cores.size() - 1 ? size : startIndex + coreBlockSize);
            std::cout << "Core: " << cores[i]->getId() << " Start: " << startIndex << " End: " << endIndex << std::endl;
            cores[i]->distributeJobToThreads(job, startIndex, endIndex);
        }
        
        for (auto &core : cores) {
            core->joinThreads();
        }
    }

};


#endif //STALKER_STLKR_THREAD_OPERATIONSLINUX_HB
