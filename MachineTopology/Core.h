//
// Created by hal9000 on 6/15/24.
//

#ifndef STALKER_CORE_H
#define STALKER_CORE_H

#include <vector>
#include <list>
#include "Thread.h"

class Core{
public:
    Core(unsigned id, std::vector<Thread*> threads);
    ~Core() = default;
    std::vector<Thread*> getThreads() const;
    void getThreads(std::vector<Thread*> &threads) const;
    void addThreadsToPool(std::list<Thread*> &threadPool, bool hyperThreaded) const;
    void addThreadsToPool(std::list<Thread*> &slaveThreadPool, std::list<Thread*> &stokerThreadPool) const;
    unsigned getAvailableThreadsCount() const;
    void setThreadAffinity();
    void setThreadAffinity(cpu_set_t &coreSet);
    void resetThreadAffinity();
    unsigned getId() const;
    void joinThreads() const;
    void printCPUSet(bool printBitSet) const;
private:
    unsigned _id;
    std::vector<Thread*> _threads;
    cpu_set_t _thisCoreSet{};
    bool _isThreadAffinitySet = false;
    bool _isHyperThreaded = false;
};
#endif //STALKER_CORE_H

