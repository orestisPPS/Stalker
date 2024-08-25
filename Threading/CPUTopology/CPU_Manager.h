//
// Created by hal9000 on 7/5/24.
//

#ifndef STALKER_CPU_MANAGER_H
#define STALKER_CPU_MANAGER_H
#include "CPU.h"
#include <unordered_map>
#include <mutex>
#include "../StalkerVector/AVX2/MemoryTraits.h"

class CPU_Manager {
public:
    CPU_Manager() {
        _hyperthreadingEnabled = false;
        _cpu = new CPU();
        _populate();
        setAvailableCores(1);
    }
    explicit CPU_Manager(unsigned availableCores, bool enableHyperthreading = false) {
        _hyperthreadingEnabled = enableHyperthreading;
        _cpu = new CPU();
        _populate();
        setAvailableCores(availableCores);
    }
    
    ~CPU_Manager() {
        delete _cpu;
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
    void release(const std::vector<Core*>& cores)  {
        auto mutex = std::mutex();
        std::lock_guard<std::mutex> lock(mutex); // Lock the mutex
        for (const auto &core : cores) {
            _corePool[core] = true;
            for (const auto &thread : core->getThreads()) {
                _threadPool[thread] = true;
                if (core->getThreads().size() > 1)
                    _hyperThreadCorePool[core] = true;
                else
                    _ecoCorePool[core] = true;
            }
        }
    }
    
    void enableHyperthreading(bool isEnabled) { _hyperthreadingEnabled = isEnabled; }
    
    bool isHyperthreadingEnabled() const { return _hyperthreadingEnabled; }
    
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
    bool _hyperthreadingEnabled;
    unsigned _availableCores;
    CPU *_cpu;
    std::unordered_map<Core*, bool> _corePool;
    std::unordered_map<Core*, bool> _hyperThreadCorePool;
    std::unordered_map<Core*, bool> _ecoCorePool;
    std::unordered_map<Thread*, bool> _threadPool;
    std::vector<Core*> _cores;
    std::vector<Thread*> _threads;
    bool _temporalStore = false;
    
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
        }
    }
};


#endif //STALKER_CPU_MANAGER_H
