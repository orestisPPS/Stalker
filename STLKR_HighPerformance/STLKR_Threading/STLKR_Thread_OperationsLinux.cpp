//
// Created by hal9000 on 6/19/24.
//

#include "STLKR_Thread_OperationsLinux.h"

void STLKR_Thread_OperationsLinux::_setThreadAffinity(const std::vector<STLKR_Machine_Core> &cores, bool enableHyperThreading) {
    
    for (const auto &core : cores) {
        cpu_set_t thisCoreSet;
        CPU_ZERO(&thisCoreSet);
        auto threads = core.getThreads();
        for (int i = 0; i < threads.size(); ++i) {
            pthread_t pThread = pthread_t();
            pThread = pthread_self();
            int result = pthread_setaffinity_np(pThread, sizeof(cpu_set_t), &thisCoreSet);
            if (result != 0) {
                std::cerr << "Error setting affinity for thread " << i << std::endl;
            } else {
                std::cout << "Thread " << i << " set to core " << core.getId() << std::endl;
            }
        }
    }
}
