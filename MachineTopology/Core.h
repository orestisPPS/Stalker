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
    void setHyperThreading(bool isHyperThreaded);
    std::vector<Thread *> getThreads() const;
    void getThreads(std::vector<Thread*> &threads) const;
    void addThreadsToPool(std::list<Thread*> &threadPool) const;
    std::vector<Thread *> getSlaveThreads() const;
    void getSlaveThreads(std::vector<Thread*> &threads) const;
    void addSlaveThreadsToPool(std::list<Thread*> &threadPool) const;
    std::vector<Thread *> getStokerThreads() const;
    void getStokerThreads(std::vector<Thread*> &threads) const;
    void addStokerThreadsToPool(std::list<Thread*> &threadPool) const;
    unsigned getThreadCount() const;
    void setThreadAffinity();
    void setThreadAffinity(cpu_set_t &coreSet);
    void resetThreadAffinity();
    unsigned getId() const;
    void joinThreads() const;
    void printCPUSet(bool printBitSet) const;

    template <typename threadJob>
    void distributeJobToThreads(threadJob job, unsigned startIndex, unsigned endIndex) {
        unsigned totalRange = endIndex - startIndex;
        unsigned threadBlockSize = totalRange / _threads.size();
        auto availableThreads = _isHyperThreaded ? _threads : std::vector<Thread*>{_threads[0]};

        for (int i = 0; i < availableThreads.size(); ++i) {
            unsigned threadStartIndex = startIndex + i * threadBlockSize;
            unsigned threadEndIndex = (i == availableThreads.size() - 1) ? endIndex : threadStartIndex + threadBlockSize;

            //std::cout << "Thread: " << _threads[i]->getId() << " Start: " << threadStartIndex << " End: " << threadEndIndex << std::endl;
            availableThreads[i]->executeJob(job, threadStartIndex, threadEndIndex, _thisCoreSet);
        }
        for (const auto &thread : availableThreads)
            thread->join();
    }



    
private:
    unsigned _id;
    std::vector<Thread*> _threads;
    cpu_set_t _thisCoreSet{};
    bool _isThreadAffinitySet = false;
    bool _isHyperThreaded = false;
    
    
    
};
#endif //STALKER_CORE_H
