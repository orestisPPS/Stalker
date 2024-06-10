//
// Created by hal9000 on 6/10/24.
//

#ifndef STALKER_STLKR_THREAD_LINUXPROCESSORSPECSREADER_H
#define STALKER_STLKR_THREAD_LINUXPROCESSORSPECSREADER_H

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

class STLKR_Thread_LinuxProcessorSpecsReader {
private:
    unsigned num_physical_cores;
    unsigned num_logical_processors;
    std::unordered_map<unsigned, std::vector<unsigned>> core_info; // key: core_id, value: list of logical processors
    std::unordered_map<unsigned, std::pair<unsigned, unsigned>> clock_info; // key: core_id, value: pair of min and max frequency
    std::unordered_map<unsigned, std::unordered_map<unsigned, std::pair<unsigned , std::vector<unsigned>>>> cache_info; // key: core_id, value: unordered_map of cache_level to (size, shared CPUs)

    static unsigned read_integer_from_file(const std::string& path);
    static std::string read_string_from_file(const std::string& path);
    static std::vector<unsigned> parse_cpu_list(const std::string& cpu_list);

    void read_cpu_info();
    void read_cache_info();

public:
    STLKR_Thread_LinuxProcessorSpecsReader();
    void initialize();
    std::unordered_map<unsigned, std::vector<unsigned>> get_physical_cores() const;
    std::unordered_map<unsigned, std::pair<unsigned, unsigned>> get_clock_info() const;
    std::unordered_map<unsigned, std::unordered_map<unsigned, std::pair<unsigned , std::vector<unsigned>>>> get_cache_info() const;

    void print_processor_specs() const;
};


#endif //STALKER_STLKR_THREAD_LINUXPROCESSORSPECSREADER_H
