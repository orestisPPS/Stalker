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
    _thisCoreSet = {};
}

unsigned Core::getId() const{
    return _id;
}

void Core::setHyperThreading(bool isHyperThreaded) {
    _isHyperThreaded = isHyperThreaded;
}

std::vector<Thread *> Core::getThreads() const{
    if (_isHyperThreaded)
        return _threads;
    else
        return {_threads[0]};
}

void Core::getThreads(std::vector<Thread*> &threads) const{
    if (_isHyperThreaded)
        threads = _threads;
    else
        threads = {_threads[0]};
}

void Core::addThreadsToPool(std::list<Thread*> &threadPool) const {
    if (_isHyperThreaded)
        threadPool.insert(threadPool.end(), _threads.begin(), _threads.end());
    else
        threadPool.push_back(_threads[0]);
}

std::vector<Thread *> Core::getSlaveThreads() const{
    auto slaveThreads = std::vector<Thread*>(_threads.size() - 1);
    std::copy(_threads.begin(), _threads.end() - 1, slaveThreads.begin());
    return slaveThreads;
}

void Core::getSlaveThreads(std::vector<Thread*> &threads) const{
    threads = std::vector<Thread*>(_threads.size() - 1);
    std::copy(_threads.begin(), _threads.end() - 1, threads.begin());
}

void Core::addSlaveThreadsToPool(std::list<Thread*> &threadPool) const {
    threadPool.insert(threadPool.end(), _threads.begin(), _threads.end() - 1);
}

std::vector<Thread *> Core::getStokerThreads() const{
    return std::vector<Thread*>{_threads[_threads.size() - 1]};
}

void Core::getStokerThreads(std::vector<Thread*> &threads) const{
    threads = std::vector<Thread*>{_threads[_threads.size() - 1]};
}

void Core::addStokerThreadsToPool(std::list<Thread*> &threadPool) const {
    threadPool.push_back(_threads[_threads.size() - 1]);
}

unsigned Core::getAvailableThreadsCount() const{
    if (_isHyperThreaded)
        return _threads.size();
    else
        return 1;
}

void Core::setThreadAffinity() {
    CPU_ZERO(&_thisCoreSet);
    auto availableThreads = _isHyperThreaded ? _threads.size() : 1;
    for (int i = 0; i < availableThreads; ++i) {
        _threads[i]->setThreadAffinity(_thisCoreSet);
    }
}

void Core::setThreadAffinity(cpu_set_t &coreSet) {
    auto availableThreads = _isHyperThreaded ? _threads.size() : 1;
    for (int i = 0; i < availableThreads; ++i) {
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
