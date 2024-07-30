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
#include "Core.h"
#include "CacheLevel.h"

class CPUTopologyLinux {

public:
    CPUTopologyLinux(std::string cpuPath = "/sys/devices/system/cpu/");
    ~CPUTopologyLinux();
    void print_processor_specs() const;
    const std::vector<Core*> &getCores(unsigned numCores = 0) const;
    const std::vector<Thread *> &getThreads() const;
    const std::vector<CacheLevel*> &getCacheLevels() const;
    const std::vector<SharedCache*> &getSharedCaches() const;

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
    static unsigned _readIntegerFromFile(const std::string& path);
    static std::string _readStringFromFile(const std::string& path);
    static std::vector<unsigned> _parseCPUList(const std::string& cpu_list);
    std::string _getThreadPath(unsigned &threadId) const;
    std::string _getCacheLevelPath(unsigned threadId, unsigned cacheIndex) const;
    void _readMachineTopology();
    void _readCacheInfo();
    CacheLevelType _getCacheLevelType(const std::string &type) const;
};


#endif //STALKER_CPUTOPOLOGYLINUX_H
