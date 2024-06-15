#include <unordered_set>
#include <list>
#include "STLKR_Machine_CPUEntitiesFactoryLinux.h"

unsigned STLKR_Machine_CPUEntitiesFactoryLinux::read_integer_from_file(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + path);
    }
    unsigned value;
    file >> value;
    return value;
}

std::string STLKR_Machine_CPUEntitiesFactoryLinux::read_string_from_file(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + path);
    }
    std::string value;
    std::getline(file, value);
    return value;
}

// Helper function to parse CPU lists like "0-3,5,7-8"
std::vector<unsigned> STLKR_Machine_CPUEntitiesFactoryLinux::parse_cpu_list(const std::string& cpu_list) {
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

STLKR_Machine_CPUEntitiesFactoryLinux::STLKR_Machine_CPUEntitiesFactoryLinux()
        : num_physical_cores(0), availableThreads(0) {}

void STLKR_Machine_CPUEntitiesFactoryLinux::initialize() {
    auto startTime = std::chrono::high_resolution_clock::now();
    getMachineCores();
    read_cache_info();
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
    std::cout << "Processor specs read in " << duration << " μs" << std::endl;
}

//void STLKR_Machine_CPUEntitiesFactoryLinux::getMachineCores() {
//    std::string online_path = "/sys/devices/system/cpu/online";
//    std::string online_cpus = read_string_from_file(online_path);
//    auto logical_cpus = parse_cpu_list(online_cpus);
//    
//    num_logical_processors = logical_cpus.size();
//    std::map<unsigned, std::vector<unsigned>> temp_core_info;
//
//    // Temporary set to keep track of already processed logical CPUs
//    std::unordered_set<unsigned> processed_cores;
//
//    for (unsigned cpu : logical_cpus) {
//        std::string core_id_path = "/sys/devices/system/cpu/cpu" + std::to_string(cpu) + "/topology/core_id";
//        unsigned core_id = read_integer_from_file(core_id_path);
//
//        if (processed_cores.find(cpu) != processed_cores.end()) {
//            continue;
//        }
//
//        // Find all siblings (hyper-threaded or eco threads) that share the same core_id
//        std::vector<unsigned> siblings;
//        for (unsigned sibling_cpu : logical_cpus) {
//            std::string sibling_core_id_path = "/sys/devices/system/cpu/cpu" + std::to_string(sibling_cpu) + "/topology/core_id";
//            unsigned sibling_core_id = read_integer_from_file(sibling_core_id_path);
//
//            if (sibling_core_id == core_id) {
//                siblings.push_back(sibling_cpu);
//                processed_cores.insert(sibling_cpu);
//            }
//        }
//
//        temp_core_info[core_id] = siblings;
//
//        // Read clock frequencies for each logical processor in the core
//        unsigned min_freq = 0;
//        unsigned max_freq = 0;
//        for (unsigned sibling_cpu : siblings) {
//            std::string min_freq_path = "/sys/devices/system/cpu/cpu" + std::to_string(sibling_cpu) + "/cpufreq/cpuinfo_min_freq";
//            std::string max_freq_path = "/sys/devices/system/cpu/cpu" + std::to_string(sibling_cpu) + "/cpufreq/cpuinfo_max_freq";
//
//            unsigned sibling_min_freq = read_integer_from_file(min_freq_path);
//            unsigned sibling_max_freq = read_integer_from_file(max_freq_path);
//
//            min_freq = std::min(min_freq, sibling_min_freq);
//            max_freq = std::max(max_freq, sibling_max_freq);
//        }
//
//        clock_info[core_id] = {min_freq, max_freq};
//    }
//
//    // Assign a new sequential physical core ID for each unique core_id found
//    unsigned physical_core_id = 0;
//    for (const auto& [core_id, logical_cpus] : temp_core_info) {
//        core_info[physical_core_id++] = logical_cpus;
//    }
//
//    num_physical_cores = core_info.size();
//}

std::vector<STLKR_Machine_Core> STLKR_Machine_CPUEntitiesFactoryLinux::getMachineCores() {


    unsigned int threadId = 0;
    unsigned int coreTopologyId = 0;
    // Find the online Threads
    auto onlineThreads = parse_cpu_list(read_string_from_file("/sys/devices/system/cpu/online"));
    availableThreads = onlineThreads.size();
    
    auto threadsVector = std::vector<STLKR_Machine_Thread>(availableThreads);
    
    auto physicalCoreToThreads = std::map<unsigned, std::vector<unsigned>>();
    for (unsigned cpu : onlineThreads) {
        threadsVector[cpu] = STLKR_Machine_Thread(threadId,
                                                  read_integer_from_file("/sys/devices/system/cpu/cpu" + std::to_string(cpu) + "/topology/core_id"),
                                                  read_integer_from_file("/sys/devices/system/cpu/cpu" + std::to_string(cpu) + "/cpufreq/cpuinfo_min_freq"));
        threadId++;
        coreTopologyId = read_integer_from_file("/sys/devices/system/cpu/cpu" + std::to_string(cpu) + "/topology/core_id");
        auto threadSiblings = parse_cpu_list(read_string_from_file("/sys/devices/system/cpu/cpu" + std::to_string(cpu) + "/topology/thread_siblings_list"));
        if (physicalCoreToThreads.find(coreTopologyId) == physicalCoreToThreads.end())
            physicalCoreToThreads[coreTopologyId] = std::move(threadSiblings);
    }
    auto machineCores = std::vector<STLKR_Machine_Core>();
    for (const auto& [coreId, threads] : physicalCoreToThreads) {
        auto coreThreads = std::vector<STLKR_Machine_Thread>();
        for (auto iThread : threads) {
            coreThreads.push_back(threadsVector[iThread]);
        }
        machineCores.emplace_back(coreId, coreThreads);
    }
    return machineCores;
}


void STLKR_Machine_CPUEntitiesFactoryLinux::read_cache_info() {
    unsigned int level, size_kb, size_bytes;
    std::string type, shared_cpus_str;
    std::vector<unsigned> shared_cpus;
    for (const auto& [core_id, cpus] : core_info) {
        for (unsigned cpu : cpus) {
            std::map<unsigned , std::pair<unsigned , std::vector<unsigned >>> core_cache_info;
            for (unsigned cache_index = 0; cache_index <= 3; ++cache_index) { // Iterate over 0, 1, 2, and 3

                std::string cache_level_path = "/sys/devices/system/cpu/cpu" + std::to_string(cpu) + "/cache/index" + std::to_string(cache_index) + "/level";
                std::string cache_type_path = "/sys/devices/system/cpu/cpu" + std::to_string(cpu) + "/cache/index" + std::to_string(cache_index) + "/type";
                std::string cache_size_path = "/sys/devices/system/cpu/cpu" + std::to_string(cpu) + "/cache/index" + std::to_string(cache_index) + "/size";
                std::string cache_shared_path = "/sys/devices/system/cpu/cpu" + std::to_string(cpu) + "/cache/index" + std::to_string(cache_index) + "/shared_cpu_list";

                if (!std::ifstream(cache_level_path).good() || !std::ifstream(cache_size_path).good() || !std::ifstream(cache_shared_path).good() || !std::ifstream(cache_type_path).good()) {
                    continue;
                }

                level = read_integer_from_file(cache_level_path);
                type = read_string_from_file(cache_type_path);
                size_kb = read_integer_from_file(cache_size_path); // size is in KB
                size_bytes = size_kb * 1024; // converting KB to bytes
                shared_cpus_str = read_string_from_file(cache_shared_path);
                shared_cpus = parse_cpu_list(shared_cpus_str);

                if (level == 1 && (type == "Data" || type == "Instruction")) {
                    if (core_cache_info.find(level) != core_cache_info.end()) {
                        core_cache_info[level].first += size_bytes;
                    } else {
                        core_cache_info[level] = {size_bytes, shared_cpus};
                    }
                } else {
                    core_cache_info[level] = {size_bytes, shared_cpus};
                }
                shared_cpus.clear();
            }
            cache_info[core_id] = core_cache_info;
        }
    }
    std::cout << "Cache info read successfully!" << std::endl;
}


std::map<unsigned, std::vector<unsigned>> STLKR_Machine_CPUEntitiesFactoryLinux::get_physical_cores() const {
    return core_info;
}

std::map<unsigned, std::pair<unsigned, unsigned>> STLKR_Machine_CPUEntitiesFactoryLinux::get_clock_info() const {
    return clock_info;
}

std::map<unsigned, std::map<unsigned, std::pair<unsigned , std::vector<unsigned>>>> STLKR_Machine_CPUEntitiesFactoryLinux::get_cache_info() const {
    return cache_info;
}

void STLKR_Machine_CPUEntitiesFactoryLinux::print_processor_specs() const {
    // Print Physical Cores and Logical Processors
    std::cout << "Physical Cores and Logical Processors:\n";
    for (const auto& core : core_info) {
        std::cout << "Core " << core.first << ": ";
        for (auto cpu : core.second) {
            std::cout << cpu << " ";
        }
        std::cout << "\n";
    }

    // Print Clock Information
    std::cout << "\nClock Information:\n";
    for (const auto& clock : clock_info) {
        std::cout << "Core " << clock.first << ": Min Frequency = " << clock.second.first << " kHz, Max Frequency = " << clock.second.second << " kHz\n";
    }

    // Print Cache Information
    std::cout << "\nCache Information:\n";
    for (const auto& core_cache : cache_info) {
        std::cout << "Core " << core_cache.first << ":\n";
        for (const auto& cache_level : core_cache.second) {
            auto& cache = cache_level.second;
            std::cout << "  L" << cache_level.first << " Cache: Size = " << cache.first / 1024 << " KB, Shared with CPUs = ";
            for (auto cpu : cache.second) {
                std::cout << cpu << " ";
            }
            std::cout << "\n";
        }
    }
}


