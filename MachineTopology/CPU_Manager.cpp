//
// Created by hal9000 on 7/5/24.
//

#include <cassert>
#include <stdexcept>
#include "CPU_Manager.h"


CPU_Manager::CPU_Manager() {
    _hyperthreadingEnabled = false;
    _cpu = new CPUTopologyLinux();
    _populate();
    setAvailableCores(1);
}

CPU_Manager::CPU_Manager(unsigned availableCores, bool enableHyperthreading){
    _hyperthreadingEnabled = enableHyperthreading;
    _cpu = new CPUTopologyLinux();
    _populate();
    setAvailableCores(availableCores);
}

CPU_Manager::~CPU_Manager() {
    delete _cpu;
}

std::vector<Core *> CPU_Manager::getCores() {
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


void CPU_Manager::release(const std::vector<Core*>& cores) {
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

void CPU_Manager::printInConsole() { _cpu->print_processor_specs(); }

void CPU_Manager::enableHyperthreading(bool isEnabled) { _hyperthreadingEnabled = isEnabled; }

bool CPU_Manager::isHyperthreadingEnabled() const { return _hyperthreadingEnabled; }

void CPU_Manager::setAvailableCores(unsigned int availableCores) {
    if (availableCores > _cores.size()){
        std::cout<<"WARNING: Available Cores requested surpass the maximum number of hyperthreaded cores."
                   " Automatically set to maximum available :" << _hyperThreadCorePool.size() <<std::endl;
    }
    _availableCores = availableCores;
}

unsigned CPU_Manager::getAvailableCores() const { return _availableCores; }

void CPU_Manager::_populate() {
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



        


            