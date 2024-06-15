//
// Created by hal9000 on 6/10/24.
//

#ifndef STALKER_STLKR_MACHINE_CPUENTITIESFACTORYLINUX_H
#define STALKER_STLKR_MACHINE_CPUENTITIESFACTORYLINUX_H

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

class STLKR_Machine_CPUEntitiesFactoryLinux {
private:
    unsigned num_physical_cores;
    unsigned availableThreads;
    std::map<unsigned, std::vector<unsigned>> core_info; // key: core_id, value: list of logical processors
    std::map<unsigned, std::pair<unsigned, unsigned>> clock_info; // key: core_id, value: pair of min and max frequency
    std::map<unsigned, std::map<unsigned, std::pair<unsigned , std::vector<unsigned>>>> cache_info; // key: core_id, value: unordered_map of cache_level to (size, shared CPUs)

    static unsigned read_integer_from_file(const std::string& path);
    static std::string read_string_from_file(const std::string& path);
    static std::vector<unsigned> parse_cpu_list(const std::string& cpu_list);

    std::vector<STLKR_Machine_Core> getMachineCores();
    void read_cache_info();

public:
    STLKR_Machine_CPUEntitiesFactoryLinux();
    void initialize();
    std::map<unsigned, std::vector<unsigned>> get_physical_cores() const;
    std::map<unsigned, std::pair<unsigned, unsigned>> get_clock_info() const;
    std::map<unsigned, std::map<unsigned, std::pair<unsigned , std::vector<unsigned>>>> get_cache_info() const;

    void print_processor_specs() const;
};


#endif //STALKER_STLKR_MACHINE_CPUENTITIESFACTORYLINUX_H
