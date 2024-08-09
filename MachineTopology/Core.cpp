//
// Created by hal9000 on 6/15/24.
//

#include <iostream>
#include <cstring>
#include <bitset>
#include "Core.h"

Core::Core(unsigned id, std::vector<Thread*> threads) {
    _id = id;
    _threads = threads;
    _isHyperThreaded = _threads.size() > 1;
    _thisCoreSet = {};
}

unsigned Core::getId() const{
    return _id;
}

std::vector<Thread *> Core::getThreads() const{
    return _threads;
}

void Core::getThreads(std::vector<Thread*> &threads) const{
    threads = _threads;
}

void Core::addThreadsToPool(std::list<Thread*> &threadPool, bool hyperThreaded) const {
    if (hyperThreaded)
        threadPool.insert(threadPool.end(), _threads.begin(), _threads.end());
    else
        threadPool.push_back(_threads[0]);
}

void Core::addThreadsToPool(std::list<Thread *> &slaveThreadPool, std::list<Thread *> &stokerThreadPool) const {
    slaveThreadPool.insert(slaveThreadPool.end(), _threads.begin(), _threads.end() - 1);
    stokerThreadPool.insert(stokerThreadPool.end(), _threads.begin(), _threads.end() - 1);
}


unsigned Core::getAvailableThreadsCount() const{
    return _threads.size();
}

void Core::setThreadAffinity() {
    CPU_ZERO(&_thisCoreSet);
    for (auto & _thread : _threads) {
        _thread->setThreadAffinity(_thisCoreSet);
    }
}

void Core::setThreadAffinity(cpu_set_t &coreSet) {
    for (int i = 0; i < _threads.size(); ++i) {
        _threads[i]->setThreadAffinity(coreSet);
    }
}

void Core::resetThreadAffinity() {
    auto availableThreads = _isHyperThreaded ? _threads.size() : 1;
    for (int i = 0; i < availableThreads; ++i) {
        _threads[i]->resetThreadAffinity();
    }
}

void Core::joinThreads() const {
    for (const auto &thread : _threads)
        thread->join();
}

void Core::printCPUSet(bool printBitSet) const {
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

