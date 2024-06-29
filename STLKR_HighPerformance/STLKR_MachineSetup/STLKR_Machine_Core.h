//
// Created by hal9000 on 6/15/24.
//

#ifndef STALKER_STLKR_MACHINE_CORE_H
#define STALKER_STLKR_MACHINE_CORE_H

#include <vector>
#include "STLKR_Machine_Thread.h"

class STLKR_Machine_Core{
public:
    STLKR_Machine_Core(unsigned id, std::vector<STLKR_Machine_Thread*> threads);
    ~STLKR_Machine_Core() = default;
    void setHyperThreading(bool isHyperThreaded);
    std::vector<STLKR_Machine_Thread *> getThreads() const;
    std::vector<STLKR_Machine_Thread *> getSlaveThreads() const;
    std::vector<STLKR_Machine_Thread *> getStokerThreads() const;
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
        auto availableThreads = _isHyperThreaded ? _threads : std::vector<STLKR_Machine_Thread*>{_threads[0]};

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
    std::vector<STLKR_Machine_Thread*> _threads;
    cpu_set_t _thisCoreSet{};
    bool _isThreadAffinitySet = false;
    bool _isHyperThreaded = false;
    
    
    
};
#endif //STALKER_STLKR_MACHINE_CORE_H

