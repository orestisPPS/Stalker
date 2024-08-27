//
// Created by hal9000 on 6/15/24.
//

#ifndef STALKER_CORE_H
#define STALKER_CORE_H

#include <utility>
#include <vector>
#include <list>
#include <bitset>
#include "Thread.h"

class Core{
public:
    Core(unsigned id, std::vector<Thread*> threads) :
        _id(id), _threads(std::move(threads)), _isHyperThreaded(_threads.size() > 1), _thisCoreSet({}) {}
        
    ~Core() = default;
    
    [[nodiscard]] const std::vector<Thread*>& getThreads() const { return _threads; }
    
    inline void getThreads(std::vector<Thread*> &threads) const { threads = _threads;}

    inline void addThreadsToPool(std::list<Thread*> &threadPool, bool hyperThreaded) const {
        if (hyperThreaded)
            threadPool.insert(threadPool.end(), _threads.begin(), _threads.end());
        else
            threadPool.push_back(_threads[0]);
    }

    inline void addThreadsToPool(std::list<Thread*> &slaveThreadPool, std::list<Thread*> &stokerThreadPool) const {
        slaveThreadPool.insert(slaveThreadPool.end(), _threads.begin(), _threads.end() - 1);
        stokerThreadPool.insert(stokerThreadPool.end(), _threads.begin(), _threads.end() - 1);
    }
    
    [[nodiscard]] inline unsigned getAvailableThreadsCount() const { return _threads.size(); }

    inline void setThreadAffinity() {
        CPU_ZERO(&_thisCoreSet);
        for (auto & _thread : _threads)
            _thread->addToCoreSet(_thisCoreSet);
        for (auto & _thread : _threads)
            _thread->setThreadAffinity(_thisCoreSet);
    }
    
    inline cpu_set_t * getCpuSet(){ return &_thisCoreSet; }
    
    [[nodiscard]] inline unsigned getId() const { return _id; }

    inline void joinThreads() const {
        for (const auto &thread : _threads)
            thread->join();
    }

    inline void printCPUSet(bool printBitSet) const {
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

    template <typename threadJob>
    inline void distributeJobToThreads(threadJob job, unsigned startIndex, unsigned endIndex, bool hyperThreadingEnabled) {
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

