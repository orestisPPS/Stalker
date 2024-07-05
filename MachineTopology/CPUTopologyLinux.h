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
#include "Core.h"
#include "CacheLevel.h"

class CPUTopologyLinux {

public:
    explicit CPUTopologyLinux(std::string cpuPath = "/sys/devices/system/cpu/");
    ~CPUTopologyLinux();
    void print_processor_specs() const;
    std::vector<Core*> getPhysicalCores() const;
    std::vector<Thread*> getThreads() const;
    std::vector<CacheLevel*> getCacheLevels() const;
    std::vector<SharedCache*> getSharedCaches() const;

private:
    std::string _cpuPath;
    std::vector<Core*> _physicalCores;
    std::vector<Thread*> _threads;
    std::vector<CacheLevel*> _cacheLevels;
    std::vector<SharedCache*> _sharedCaches;
    void _readThreads();
    void _readCores();
    void _readCacheLevels();

    //------------------------------------------------------------------
    //Reader functions
    //------------------------------------------------------------------
    static unsigned _readIntegerFromFile(const std::string& path);
    static std::string _readStringFromFile(const std::string& path);
    static std::vector<unsigned> _parseCPUList(const std::string& cpu_list);
    std::string _getThreadPath(unsigned &threadId) const;
    std::string _getCacheLevelPath(unsigned threadId, unsigned cacheIndex) const;
    void readMachineCores();
    void _readCacheInfo();

};


#endif //STALKER_CPUTOPOLOGYLINUX_H
