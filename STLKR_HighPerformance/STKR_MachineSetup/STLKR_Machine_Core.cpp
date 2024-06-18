//
// Created by hal9000 on 6/15/24.
//

#include <iostream>
#include "STLKR_Machine_Core.h"

STLKR_Machine_Core::STLKR_Machine_Core(unsigned id, std::vector<STLKR_Machine_Thread*> threads) {
    _id = id;
    _threads = std::move(threads);
    _pThreads = std::vector<pthread_t>(_threads.size());
    setThreadAffinity();
    
}

unsigned STLKR_Machine_Core::getId() const{
    return _id;
}

const std::vector<STLKR_Machine_Thread *> & STLKR_Machine_Core::getThreads() const{
    return _threads;
}

void STLKR_Machine_Core::setThreadAffinity() {
    cpu_set_t thisCoreSet;
    CPU_ZERO(&thisCoreSet);
    for (auto & _thread : _threads) {
        CPU_SET(_thread->getId(), &thisCoreSet);
    }
    
//    for (int i = 0; i < _pThreads.size(); ++i) {
//        _pThreads[i] = pthread_t();
//        _pThreads[i] = pthread_self();
//        std::cout << "pthread_self() = " << _pThreads[i] << std::endl;
//        int result = pthread_setaffinity_np(_pThreads[i], sizeof(cpu_set_t), &thisCoreSet);
//        if (result != 0) {
//            std::cerr << "Error setting affinity for thread " << i << std::endl;
//        } else {
//            std::cout << "Thread " << i << " set to core " << _id << std::endl;
//        }
//    }
}




