//
// Created by hal9000 on 6/10/24.
//

#ifndef STALKER_STLKR_MACHINE_CPUTOPOLOGYLINUX_H
#define STALKER_STLKR_MACHINE_CPUTOPOLOGYLINUX_H

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
#include "STLKR_Machine_Core.h"
#include "STLKR_Machine_CacheLevel.h"

class STLKR_Machine_CPUTopologyLinux {

public:
    explicit STLKR_Machine_CPUTopologyLinux(std::string cpuPath = "/sys/devices/system/cpu/");
    
    void print_processor_specs() const;

private:
    std::string _cpuPath;
    std::vector<STLKR_Machine_Core*> _physicalCores;
    std::vector<STLKR_Machine_Thread*> _threads;
    std::vector<STLKR_Machine_CacheLevel*> _cacheLevels;
    std::vector<STLKR_Machine_SharedCache*> _sharedCaches;
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


#endif //STALKER_STLKR_MACHINE_CPUTOPOLOGYLINUX_H
