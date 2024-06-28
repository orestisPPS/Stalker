//
// Created by hal9000 on 6/15/24.
//

#include <iostream>
#include <cstring>
#include <bitset>
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
    auto availableThreads = _isHyperThreaded ? _threads.size() : 1;
    for (int i = 0; i < availableThreads; ++i) {
        _threads[i]->setThreadAffinity(_thisCoreSet, 0);
    }
    printCPUSet(false);
}

void STLKR_Machine_Core::resetThreadAffinity() {
    auto availableThreads = _isHyperThreaded ? _threads.size() : 1;
    for (int i = 0; i < availableThreads; ++i) {
        _threads[i]->resetThreadAffinity();
    }
}

void STLKR_Machine_Core::joinThreads() const {
    for (const auto &thread : _threads)
        thread->join();
}

void STLKR_Machine_Core::printCPUSet(bool printBitSet) const {
    std::cout << "CPU Set for core " << _id << ": " << std::endl;

    if (printBitSet) {
        std::bitset<sizeof(cpu_set_t) * 8> bits(*reinterpret_cast<const unsigned long*>(&_thisCoreSet));
        std::string bitString = bits.to_string();
        for (size_t i = 0; i < bitString.size(); i += 8) {
            std::cout << bitString.substr(i, 8) << " ";
        }
        std::cout << std::endl;
    }

    // Print the CPUs that are part of the set
    std::cout << "CPUs included in the set: ";
    for (int i = 0; i < CPU_SETSIZE; ++i) {
        if (CPU_ISSET(i, &_thisCoreSet)) {
            std::cout << i << " ";
        }
    }
    std::cout << std::endl;
}
