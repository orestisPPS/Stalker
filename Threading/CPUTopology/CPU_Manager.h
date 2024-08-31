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
    HT_CoreSet,                // Hyperthreading is enabled and all core threads are bound to a single cpu_set_t.
    HT_PoolSet,                // Hyperthreading is enabled and all core threads are bound to a single cpu_set_t.
    SingleThread_CoreSet,         // Threads are bound to a pool of CPU cores in a single cpu_set_t.
    SingleThread_PoolSet        // A single thread of each core is bound to it's own cpu_set_t.
};


class CPU_Manager {
public:
    CPU_Manager() {
        _affinityConfig = SingleThread_CoreSet;
        _activeSets = std::list<cpu_set_t *>();
        _cpu = new CPU();
        _populate();
        setAvailableCores(1);
    }
    explicit CPU_Manager(unsigned availableCores, bool enableHyperthreading = false) {
        _affinityConfig = SingleThread_CoreSet;
        _activeSets = std::list<cpu_set_t *>();
        _cpu = new CPU();
        _populate();
        setAvailableCores(availableCores);
    }
    
    ~CPU_Manager() {
        delete _cpu;
    }

    std::vector<Thread *> getThreadPool()  {
        auto threadPool = std::vector<Thread*>();
        bool htEnabled = false;
        if (_affinityConfig == HT_CoreSet || _affinityConfig == HT_PoolSet) htEnabled = true;
        else if (_affinityConfig == SingleThread_PoolSet ||_affinityConfig == SingleThread_CoreSet) htEnabled = false;
        auto iCore = -1;
        for (const auto &core : _htCorePool) {
            if (++iCore < _availableCores && core.second) {
                core.first->addThreadsToPool(threadPool, htEnabled);
                if (_affinityConfig == HT_CoreSet){
                    core.first->setThreadAffinity();
                    _activeSets.push_back(core.first->getCpuSet());
                }
                _htCorePool[core.first] = false;
            }
        }
        if (_affinityConfig == SingleThread_CoreSet){
            for (const auto &thread : threadPool){
                auto set = new cpu_set_t;
                CPU_ZERO(set);
                thread->addToCoreSet(set);
                _activeSets.push_back(set);
            }
        }
        else if (_affinityConfig == SingleThread_PoolSet || _affinityConfig == HT_PoolSet){
            auto set = new cpu_set_t;
            CPU_ZERO(set);
            for (const auto &thread : threadPool)
                thread->addToCoreSet(set);
            _activeSets.push_back(set);
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
        return getCoresLambda(_htCorePool);
    }

    inline void releaseResources(std::vector<Thread*>& threads)  {
        Core* parent;
        for (const auto &thread : threads){
            thread->join();
            _threadPool[thread] = true;
            parent = _threadToCoreMap[thread];
            parent->getAvailableThreadsCount() > 1 ?
                    _htCorePool[parent] = true : _ecoCorePool[parent] = true;
        }
        for (auto &setPtr : _activeSets)
            if (!setPtr) delete setPtr;
        
    }

    inline void releaseResources(std::vector<Core*>& cores)  {
        auto mutex = std::mutex();
        std::lock_guard<std::mutex> lock(mutex); // Lock the mutex
        for (const auto &core : cores){
            _corePool[core] = true;
            core->getAvailableThreadsCount() > 1 ? _htCorePool[core] = true : _ecoCorePool[core] = true;
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
            std::cout <<"WARNING: Available Cores requested surpass the maximum number of hyperthreaded cores."
                       " Automatically set to maximum available :" << _htCorePool.size() << std::endl;
        }
        _availableCores = availableCores;
    }
    
    unsigned getAvailableCores() const { return _availableCores; }
    
    void printInConsole() {  _cpu->print(); }
    
private:
    unsigned _availableCores;
    CPU *_cpu;
    std::unordered_map<Core*, bool> _corePool;
    std::unordered_map<Core*, bool> _htCorePool;
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
        _htCorePool = std::unordered_map<Core*, bool>();
        _ecoCorePool = std::unordered_map<Core*, bool>();
        _threadPool = std::unordered_map<Thread*, bool>();
        _cores = _cpu->getCores();
        _threads = _cpu->getThreads();

        for (const auto &thread : _threads)
            _threadPool.insert({thread, true});

        for (const auto &core : _cores){
            if (core->getAvailableThreadsCount() > 1)
                _htCorePool.insert({core, true});
            else
                _ecoCorePool.insert({core, true});
            _corePool.insert({core, true});
            for (auto& thread : core->getThreads())
                _threadToCoreMap.insert(std::make_pair(thread, core));
        }
    }
};


#endif //STALKER_CPU_MANAGER_H
