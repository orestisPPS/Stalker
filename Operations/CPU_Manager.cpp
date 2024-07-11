//
// Created by hal9000 on 7/5/24.
//

#include <cassert>
#include "CPU_Manager.h"


CPU_Manager::CPU_Manager() {
    _cpu = new CPUTopologyLinux();
    _corePool = std::unordered_map<Core*, bool>();
    _hyperThreadCorePool = std::unordered_map<Core*, bool>();
    _ecoCorePool = std::unordered_map<Core*, bool>();
    _threadPool = std::unordered_map<Thread*, bool>();
    _cores = _cpu->getCores();
    _threads = _cpu->getThreads();
    
    
    for (const auto &thread : _threads)
        _threadPool.insert({thread, true});
    
    for (const auto &core : _cores){
        core->setHyperThreading(true);
        if (core->getAvailableThreadsCount() > 1)
            _hyperThreadCorePool.insert({core, true});
        else
            _ecoCorePool.insert({core, true});
        _corePool.insert({core, true});
    }
}

CPU_Manager::~CPU_Manager() {
    delete _cpu;
}

std::vector<Core *> CPU_Manager::getCores(unsigned numCores) {
    std::vector<Core*> cores;
    cores.reserve(numCores);
    for (const auto &core : _corePool) {
        if (core.second) {
            cores.push_back(core.first);
            _corePool[core.first] = false;
            for (const auto &thread : core.first->getThreads())
                _threadPool[thread] = false;
            if (cores.size() == numCores)
                break;
        }
    }
    return cores;
}

Core* CPU_Manager::getCore(bool hyperThread) {
    if (hyperThread) {
        for (const auto &core : _hyperThreadCorePool) {
            if (core.second) {
                _hyperThreadCorePool[core.first] = false;
                for (const auto &thread : core.first->getThreads())
                    _threadPool[thread] = false;
                return core.first;
            }
        }
    } else {
        for (const auto &core : _ecoCorePool) {
            if (core.second) {
                _ecoCorePool[core.first] = false;
                for (const auto &thread : core.first->getThreads())
                    _threadPool[thread] = false;
                return core.first;
            }
        }
    }
    return nullptr;
}

std::vector<const Core*> CPU_Manager::getHyperThreadCores(unsigned numCores) {
    std::vector<const Core*> cores;
    cores.reserve(numCores);
    for (const auto &core : _hyperThreadCorePool) {
        if (core.second) {
            cores.push_back(core.first);
            _hyperThreadCorePool[core.first] = false;
            for (const auto &thread : core.first->getThreads())
                _threadPool[thread] = false;
            if (cores.size() == numCores)
                break;
        }
    }
    return cores;
}

std::vector<const Core*> CPU_Manager::getEcoCores(unsigned numCores) {
    std::vector<const Core*> cores;
    cores.reserve(numCores);
    for (const auto &core : _ecoCorePool) {
        if (core.second) {
            cores.push_back(core.first);
            _ecoCorePool[core.first] = false;
            for (const auto &thread : core.first->getThreads())
                _threadPool[thread] = false;
            if (cores.size() == numCores)
                break;
        }
    }
    return cores;
}

std::vector<const Thread*> CPU_Manager::getThreads(unsigned numThreads) {
    if (numThreads > _threads.size() || numThreads == 0)
        throw std::runtime_error("Invalid number of threads requested");
    std::vector<const Thread*> threads;
    threads.reserve(numThreads);
    for (const auto &thread : _threadPool) {
        if (thread.second) {
            threads.push_back(thread.first);
            _threadPool[thread.first] = false;
            const auto parentCore = _cores[thread.first->getParentId()];
            _corePool[parentCore] = false;
            if (parentCore->getThreads().size() > 1)
                _hyperThreadCorePool[parentCore] = false;
            else
                _ecoCorePool[parentCore] = false;
            if (threads.size() == numThreads)
                break;
        }
    }
    return threads;
}

Thread* CPU_Manager::getThread() {
    if (!_ecoCorePool.empty()) {
        for (const auto &thread : _threadPool) {
            if (thread.second) {
                _threadPool[thread.first] = false;
                const auto parentCore = _cores[thread.first->getParentId()];
                _corePool[parentCore] = false;
                if (parentCore->getThreads().size() > 1)
                    _hyperThreadCorePool[parentCore] = false;
                else
                    _ecoCorePool[parentCore] = false;
                return thread.first;
            }
        }
    }
    else {
        for (const auto &thread : _threadPool) {
            if (thread.second) {
                _threadPool[thread.first] = false;
                const auto parentCore = _cores[thread.first->getParentId()];
                _corePool[parentCore] = false;
                if (parentCore->getThreads().size() > 1)
                    _hyperThreadCorePool[parentCore] = false;
                else
                    _ecoCorePool[parentCore] = false;
                return thread.first;
            }
        }
    }
    return nullptr;
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

void CPU_Manager::release(std::vector<Thread*> threads) {
    auto mutex = std::mutex();
    std::lock_guard<std::mutex> lock(mutex); // Lock the mutex
    for (const auto &thread : threads) {
        _threadPool[thread] = true;
        const auto parentCore = _cores[thread->getParentId()];
        _corePool[parentCore] = true;
        if (parentCore->getThreads().size() > 1)
            _hyperThreadCorePool[parentCore] = true;
        else
            _ecoCorePool[parentCore] = true;
    }
}

        


            