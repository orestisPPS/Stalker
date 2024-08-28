//
// Created by hal9000 on 7/5/24.
//

#ifndef STALKER_CPU_MANAGER_H
#define STALKER_CPU_MANAGER_H
#include "CPU.h"
#include <unordered_map>
#include <mutex>
#include "../StalkerVector/AVX2/MemoryTraits.h"

enum CoreAffinityConfig{
    HyperThreadsEnabled,   // Hyperthreading is enabled and all core threads are bound to a single cpu_set_t.
    ThreadPoolSet,         // Threads are bound to a pool of CPU cores in a single cpu_set_t.
    SingleThreadSet        // A single thread of each core is bound to it's own cpu_set_t.
};


class CPU_Manager {
public:
    CPU_Manager() {
        _affinityConfig = SingleThreadSet;
        _activeSets = std::list<cpu_set_t *>();
        _cpu = new CPU();
        _populate();
        setAvailableCores(1);
    }
    explicit CPU_Manager(unsigned availableCores, bool enableHyperthreading = false) {
        _affinityConfig = SingleThreadSet;
        _activeSets = std::list<cpu_set_t *>();
        _cpu = new CPU();
        _populate();
        setAvailableCores(availableCores);
    }
    
    ~CPU_Manager() {
        delete _cpu;
    }

    std::list<Thread *> getThreadPool()  {
        auto threadPool = std::list<Thread*>();
        bool hyperThreadingEnabled = false;
        _affinityConfig == HyperThreadsEnabled ? hyperThreadingEnabled = true : hyperThreadingEnabled = false;
        auto iCore = -1;
        for (const auto &core : _hyperThreadCorePool) {
            if (++iCore < _availableCores && core.second) {
                core.first->addThreadsToPool(threadPool, hyperThreadingEnabled);
                if (_affinityConfig == HyperThreadsEnabled){
                    core.first->setThreadAffinity();
                    _activeSets.push_back(core.first->getCpuSet());
                }
                _hyperThreadCorePool[core.first] = false;
                _corePool[core.first] = false;
                for (const auto &thread : core.first->getThreads())
                    _threadPool[thread] = false;
            }
        }
        if (_affinityConfig == SingleThreadSet){
            for (const auto &thread : threadPool){
                cpu_set_t set;
                CPU_ZERO(&set);
                thread->addToCoreSet(set);
                thread->setThreadAffinity(set);
                _activeSets.push_back(&set);
            }
        }
        else if (_affinityConfig == ThreadPoolSet){
            cpu_set_t set;
            CPU_ZERO(&set);
            for (const auto &thread : threadPool)
                thread->addToCoreSet(set);
            for (const auto &thread : threadPool)
                thread->setThreadAffinity(set);
            _activeSets.push_back(&set);
        }
        return threadPool;
    }

    std::vector<Core *> getCores()  {
        auto getCoresLambda = [&](std::unordered_map<Core*, bool> &pool){
            auto numCores = _availableCores;
            if (numCores > pool.size()) throw std::invalid_argument("Not Sufficient Cores");
            auto cores = std::vector<Core*>();
            cores.reserve(numCores);
            auto iCore = -1;
            for (const auto &core : pool) {
                if (++iCore < numCores && core.second) {
                    cores.push_back(core.first);
                    pool[core.first] = false;
                    _corePool[core.first] = false;
                    for (const auto &thread : core.first->getThreads())
                        _threadPool[thread] = false;
                }
            }
            return cores;
        };
        return getCoresLambda(_hyperThreadCorePool);
    }

    inline void releaseResources(std::list<Thread*>& threads)  {
//        auto mutex = std::mutex();
//        std::lock_guard<std::mutex> lock(mutex); // Lock the mutex
        Core* parent;
        for (const auto &thread : threads){
            thread->resetThreadAffinity();
            thread->join();
            _threadPool[thread] = true;
            parent = _threadToCoreMap[thread];
            _corePool[parent] = true;
            if (parent->getAvailableThreadsCount() > 1)
                _hyperThreadCorePool[parent] = true;
            else
                _ecoCorePool[parent] = true;
            //parent->getAvailableThreadsCount() > 1 ? _hyperThreadCorePool[parent] = true : _ecoCorePool[parent] = true;
        }
        _activeSets.clear();
    }

    inline void releaseResources(std::vector<Core*>& cores)  {
        auto mutex = std::mutex();
        std::lock_guard<std::mutex> lock(mutex); // Lock the mutex
        for (const auto &core : cores){
            _corePool[core] = true;
            core->getAvailableThreadsCount() > 1 ? _hyperThreadCorePool[core] = true : _ecoCorePool[core] = true;
            for (auto &thread : core->getThreads())
                _threadPool[thread] = true;
        }
    }
    
    void setCoreAffinityConfig(CoreAffinityConfig config) { _affinityConfig = config; }
    
    bool getCoreAffinityConfig() const { return _affinityConfig; }
    
    unsigned getTotalCores() { return _cores.size(); }
    
    unsigned getTotalThreads() { return _threads.size(); }
    
    const std::vector<SharedCache*> & getSharedCaches() { return _cpu->getSharedCaches(); }

    const std::vector<CacheLevel*> & getCacheLevels(){ return _cpu->getCacheLevels(); }
    
    void setTemporalCacheStore(bool isTemporal) { _temporalStore = isTemporal; }
    
    bool getTemporalCacheStore() const { return _temporalStore; }
    
    void setAvailableCores(unsigned availableCores) {
        if (availableCores > _cores.size()){
            std::cout<<"WARNING: Available Cores requested surpass the maximum number of hyperthreaded cores."
                       " Automatically set to maximum available :" << _hyperThreadCorePool.size() <<std::endl;
        }
        _availableCores = availableCores;
    }
    
    unsigned getAvailableCores() const { return _availableCores; }
    
    void printInConsole() {  _cpu->print(); }
    
private:
    unsigned _availableCores;
    CPU *_cpu;
    std::unordered_map<Core*, bool> _corePool;
    std::unordered_map<Core*, bool> _hyperThreadCorePool;
    std::unordered_map<Core*, bool> _ecoCorePool;
    std::unordered_map<Thread*, bool> _threadPool;
    std::unordered_map<Thread*, Core*> _threadToCoreMap;
    std::vector<Core*> _cores;
    std::vector<Thread*> _threads;
    bool _temporalStore = false;
    CoreAffinityConfig _affinityConfig;
    std::list<cpu_set_t*> _activeSets;
    
    void _populate()  {
        _corePool = std::unordered_map<Core*, bool>();
        _hyperThreadCorePool = std::unordered_map<Core*, bool>();
        _ecoCorePool = std::unordered_map<Core*, bool>();
        _threadPool = std::unordered_map<Thread*, bool>();
        _cores = _cpu->getCores();
        _threads = _cpu->getThreads();

        for (const auto &thread : _threads)
            _threadPool.insert({thread, true});

        for (const auto &core : _cores){
            if (core->getAvailableThreadsCount() > 1)
                _hyperThreadCorePool.insert({core, true});
            else
                _ecoCorePool.insert({core, true});
            _corePool.insert({core, true});
            for (auto& thread : core->getThreads())
                _threadToCoreMap.insert(std::make_pair(thread, core));
        }
    }
};


#endif //STALKER_CPU_MANAGER_H
