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
    unsigned getThreadCount() const;
    void setThreadAffinity();
    void resetThreadAffinity();
    unsigned getId() const;
    void joinThreads() const;

    template <typename threadJob>
    void distributeJobToThreads(threadJob job, unsigned startIndex, unsigned endIndex) {
        unsigned totalRange = endIndex - startIndex;
        unsigned threadBlockSize = totalRange / _threads.size();

        for (int i = 0; i < _threads.size(); ++i) {
            unsigned threadStartIndex = startIndex + i * threadBlockSize;
            unsigned threadEndIndex = (i == _threads.size() - 1) ? endIndex : threadStartIndex + threadBlockSize;

            //std::cout << "Thread: " << _threads[i]->getId() << " Start: " << threadStartIndex << " End: " << threadEndIndex << std::endl;
            _threads[i]->executeJob(job, threadStartIndex, threadEndIndex);
        }
        for (const auto &thread : _threads)
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

