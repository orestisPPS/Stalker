//
// Created by hal9000 on 6/15/24.
//

#include <iostream>
#include <cstring>
#include "STLKR_Machine_Core.h"

STLKR_Machine_Core::STLKR_Machine_Core(unsigned id, std::vector<STLKR_Machine_Thread*> threads) {
    _id = id;
    _threads = std::move(threads);
    _thisCoreSet = {};
}

unsigned STLKR_Machine_Core::getId() const{
    return _id;
}

void STLKR_Machine_Core::setHyperThreading(bool isHyperThreaded) {
    _isHyperThreaded = isHyperThreaded;
}

std::vector<STLKR_Machine_Thread *> STLKR_Machine_Core::getThreads() const{
    if (_isHyperThreaded)
        return _threads;
    else
        return std::vector<STLKR_Machine_Thread*>{_threads[0]};
}

unsigned STLKR_Machine_Core::getThreadCount() const{
    return _threads.size();
}

void STLKR_Machine_Core::setThreadAffinity() {
    CPU_ZERO(&_thisCoreSet);
    if (_isHyperThreaded)
        for (const auto &thread : _threads){
            thread->setThreadAffinity(_thisCoreSet);
            thread->addToCoreSet(_id, _thisCoreSet);
        }
    else{
        _threads[0]->setThreadAffinity(_thisCoreSet);
        _threads[0]->addToCoreSet(_id, _thisCoreSet);
    }
}

void STLKR_Machine_Core::resetThreadAffinity() {
    if (_isHyperThreaded){
        for (const auto &thread : _threads)
            thread->resetThreadAffinity(); 
    }
    else{
        _threads[0]->resetThreadAffinity();
    }
}

void STLKR_Machine_Core::joinThreads() const {
    for (const auto &thread : _threads)
        thread->join();
}