//
// Created by hal9000 on 6/10/24.
//

#ifndef STALKER_CPUTOPOLOGYLINUX_H
#define STALKER_CPUTOPOLOGYLINUX_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <stdexcept>
#include <unistd.h>
#include <dirent.h>
#include <chrono>
#include <unordered_map>
#include <mutex>
#include <algorithm>
#include "Core.h"
#include "Cache.h"

class CPU {

public:
    explicit CPU(std::string cpuPath = "/sys/devices/system/cpu/") :_cpuPath("/sys/devices/system/cpu/") {
        _readMachineTopology();
    }
    
    ~CPU() {
        for (auto core : _physicalCores) {
            delete core;
        }
        for (auto thread : _threads) {
            delete thread;
        }
        for (auto cache : _cacheLevels) {
            delete cache;
        }
        for (auto sharedCache : _sharedCaches) {
            delete sharedCache;
        }
    }
    void print() const {
        // Print Physical Cores and Logical Processors
        std::cout << "Physical Cores and Logical Processors:\n";
        for (const auto& core : _physicalCores) {
            std::cout << "Core " << core->getId() << ": ";
            const auto& threads = core->getThreads();
            for (const auto& thread : threads) {
                std::cout << "Thread " << thread->getId() << " ";
            }
            std::cout << "\n";
        }

        // Print Clock Information
        std::cout << "\nClock Information:\n";
        for (const auto& thread : _threads) {
            if (thread != nullptr) {
                std::cout << "Thread " << thread->getId() << ": Min Frequency = "
                          << thread->getClockMin() << " kHz, Max Frequency = "
                          << thread->getClockMax () << " kHz\n";
            }
        }

        // Print Cache Information
        std::cout << "\nCache Information:\n";
        for (const auto& cacheLevel : _cacheLevels) {
            if (cacheLevel != nullptr) {
                std::cout << "L" << cacheLevel->getLevel() << " Cache: Size = "
                          << cacheLevel->getSize() << " KB, Shared with CPUs = ";
                const auto& sharedCPUs = cacheLevel->getThreads();
                for (auto cpu : sharedCPUs) {
                    std::cout << cpu << " ";
                }
                std::cout << "\n";
            }
        }

        // Print Shared Cache Information
        std::cout << "\nShared Cache Information:\n";
        for (const auto& sharedCache : _sharedCaches) {
            if (sharedCache != nullptr) {
                std::cout << "Shared Cache: L1_Data Size = " << sharedCache->getCacheLevel(L1_Data)->getSize() << " KB, "
                          << "L1_Instruction Size = " << sharedCache->getCacheLevel(L1_Instructions)->getSize() << " KB, "
                          << "L2 Size = " << sharedCache->getCacheLevel(L2)->getSize() << " KB, "
                          << "L3 Size = " << sharedCache->getCacheLevel(L3)->getSize() << " KB\n";
            }
        }

        std::cout << "\n";
    }
    const std::vector<Core*> &getCores(unsigned numCores = 0) const { return _physicalCores; }
    
    const std::vector<Thread *> &getThreads() const { return _threads; }
    
    const std::vector<CacheLevel*> &getCacheLevels() const { return _cacheLevels; }
    
    const std::vector<SharedCache*> &getSharedCaches() const { return  _sharedCaches; }

private:
    std::string _cpuPath;
    std::vector<Core*> _physicalCores;
    std::vector<Thread*> _threads;
    std::vector<CacheLevel*> _cacheLevels;
    std::vector<SharedCache*> _sharedCaches;
    
    std::unordered_map<Core*, bool> _corePool;
    std::unordered_map<Core*, bool> _hyperThreadCorePool;
    std::unordered_map<Core*, bool> _ecoCorePool;
    std::unordered_map<Thread*, bool> _threadPool;

    //std::mutex _mutex;

    //------------------------------------------------------------------
    //Reader functions
    //------------------------------------------------------------------
    void _readMachineTopology() {
        auto start = std::chrono::high_resolution_clock::now();
        unsigned int size_kb = 0, coreTopologyId = 0;
        std::string type, shared_cpus_str;

        // Find the online Threads
        auto onlineThreads = _parseCPUList(_readStringFromFile("/sys/devices/system/cpu/online"));

        _threads = std::vector<Thread*>(onlineThreads.size());
        _cacheLevels = std::vector<CacheLevel*>();
        _cacheLevels.reserve(2 * onlineThreads.size() + 1); // 2 cache levels per thread + 1 for the L3 cache
        _sharedCaches = std::vector<SharedCache*>();

        auto physicalCoreToThreads = std::unordered_map<unsigned, std::vector<unsigned>>();
        auto threadToClockSpeed = std::unordered_map<unsigned, std::pair<unsigned, unsigned>>();
        auto cacheMap = std::map<unsigned, std::map<std::vector<unsigned>, CacheLevel*>>(); //Key is the cache level, value is a map of the shared CPUs and the cache level
        auto threadToCacheLevels = std::unordered_map<unsigned, CacheLevel*[4]>();
        for (unsigned iThread : onlineThreads) {
            //Read the core id of the thread
            coreTopologyId = _readIntegerFromFile(
                    _getThreadPath(iThread) + "/topology/core_id");
            auto threadSiblings = _parseCPUList(_readStringFromFile(_getThreadPath(iThread) + "/topology/thread_siblings_list"));
            if (physicalCoreToThreads.find(coreTopologyId) == physicalCoreToThreads.end())
                physicalCoreToThreads[coreTopologyId] = std::move(threadSiblings);

            //Read the cache levels of the thread
            for (unsigned cache_index = 0; cache_index <= 3; ++cache_index) { // Iterate over 1, 2, and 3
                size_kb = _readIntegerFromFile(_getCacheLevelPath(iThread, cache_index) + "/size");
                shared_cpus_str = _readStringFromFile(_getCacheLevelPath(iThread, cache_index) + "/shared_cpu_list");
                auto sharedCPUs = _parseCPUList(shared_cpus_str);
                if (cacheMap.find(cache_index) == cacheMap.end())
                    cacheMap[cache_index] = std::map<std::vector<unsigned>, CacheLevel*>();
                if (cacheMap[cache_index].find(sharedCPUs) == cacheMap[cache_index].end()){
                    cacheMap[cache_index][sharedCPUs] = new CacheLevel(_getCacheLevelType(cache_index), size_kb, sharedCPUs);
                    _cacheLevels.push_back(cacheMap[cache_index][sharedCPUs]);
                }
                threadToCacheLevels[iThread][cache_index] = cacheMap[cache_index][sharedCPUs];
            }
            _sharedCaches.push_back(new SharedCache(threadToCacheLevels[iThread][0], threadToCacheLevels[iThread][1],
                                                    threadToCacheLevels[iThread][2], threadToCacheLevels[iThread][3]));
            _threads[iThread] = new Thread(iThread, coreTopologyId,
                                           _readIntegerFromFile(_getThreadPath(iThread) + "/cpufreq/cpuinfo_min_freq"),
                                           _readIntegerFromFile(_getThreadPath(iThread) + "/cpufreq/cpuinfo_max_freq"),
                                           _sharedCaches.back());
        }
        _physicalCores = std::vector<Core*>();
        _physicalCores.reserve(physicalCoreToThreads.size());

        for (const auto& [coreId, threads] : physicalCoreToThreads) {
            auto coreThreads = std::vector<Thread*>();
            for (auto threadID : threads){
                coreThreads.push_back(_threads[threadID]);
                _threadPool.insert({_threads[threadID], true});
            }
            auto core = new Core(coreId, std::move(coreThreads));
            core->getAvailableThreadsCount() > 1 ? _hyperThreadCorePool.insert({core, true}) : _ecoCorePool.insert({core, true});
            _physicalCores.emplace_back(core);
        }
        //sort the cores by id
        std::sort(_physicalCores.begin(), _physicalCores.end(), [](const Core* a, const Core* b) {
            return a->getId() < b->getId();
        });
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "Time to read the machine cores: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " μs" << std::endl;
        std::cout << "Cache info read successfully!" << std::endl;
    }
    
    static unsigned _readIntegerFromFile(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + path);
        }
        unsigned value;
        file >> value;
        return value;
    }
    
    static std::string _readStringFromFile(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + path);
        }
        std::string value;
        std::getline(file, value);
        return value;
    }
    
    static std::vector<unsigned> _parseCPUList(const std::string& cpu_list) {
        std::vector<unsigned> cpus;
        std::stringstream ss(cpu_list);
        std::string range;
        size_t start, end, dash;

        while (std::getline(ss, range, ',')) {
            dash = range.find('-');
            if (dash != std::string::npos) {
                start = std::stoi(range.substr(0, dash));
                end = std::stoi(range.substr(dash + 1));
                for (unsigned i = start; i <= end; ++i) {
                    cpus.push_back(i);
                }
            } else {
                cpus.push_back(std::stoi(range));
            }
        }
        return cpus;
    }
    
    std::string _getThreadPath(unsigned &threadId) const {
        return _cpuPath + "cpu" + std::to_string(threadId);
    }
    
    std::string _getCacheLevelPath(unsigned threadId, unsigned cacheIndex) const {
        return _cpuPath + "cpu" + std::to_string(threadId) + "/cache/index" + std::to_string(cacheIndex);
    }

    static CacheLevelType _getCacheLevelType(unsigned level) {
        switch (level) {
            case 0: return L1_Data;
            case 1: return L1_Instructions;
            case 2: return L2;
            case 3: return L3;
            default: throw std::invalid_argument("size E [0.3]");
        }
    }
};

#endif //STALKER_CPUTOPOLOGYLINUX_H
