#include <unordered_set>
#include <list>
#include <utility>
#include <algorithm>
#include "STLKR_Machine_CPUTopologyLinux.h"



STLKR_Machine_CPUTopologyLinux::STLKR_Machine_CPUTopologyLinux(std::string cpuPath){
    _cpuPath = std::move(cpuPath);
    readMachineCores();
    
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
//        std::string core_id_path = _getThreadPath(cpu) + "/topology/core_id";
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

void STLKR_Machine_CPUTopologyLinux::readMachineCores() {

    unsigned int size_kb = 0, coreTopologyId = 0, threadId = 0;
    std::string type, shared_cpus_str;

    // Find the online Threads
    auto onlineThreads = _parseCPUList(_readStringFromFile("/sys/devices/system/cpu/online"));

    _physicalCores = std::vector<STLKR_Machine_Core*>();
    _threads = std::vector<STLKR_Machine_Thread*>(onlineThreads.size());
    _cacheLevels = std::vector<STLKR_Machine_CacheLevel*>();
    _cacheLevels.reserve(2 * onlineThreads.size() + 1); // 2 cache levels per thread + 1 for the L3 cache
    _sharedCaches = std::vector<STLKR_Machine_SharedCache*>();
    
    auto physicalCoreToThreads = std::unordered_map<unsigned, std::vector<unsigned>>();
    auto threadToClockSpeed = std::unordered_map<unsigned, std::pair<unsigned, unsigned>>();
    auto cacheMap = std::map<unsigned, std::map<std::vector<unsigned>, STLKR_Machine_CacheLevel*>>(); //Key is the cache level, value is a map of the shared CPUs and the cache level
    auto threadToCacheLevels = std::unordered_map<unsigned, STLKR_Machine_CacheLevel*[3]>();
    for (unsigned cpu : onlineThreads) {
        //Read the core id of the thread
        coreTopologyId = _readIntegerFromFile(
                _getThreadPath(cpu) + "/topology/core_id");
        auto threadSiblings = _parseCPUList(_readStringFromFile(_getThreadPath(cpu) + "/topology/thread_siblings_list"));
        if (physicalCoreToThreads.find(coreTopologyId) == physicalCoreToThreads.end())
            physicalCoreToThreads[coreTopologyId] = std::move(threadSiblings);
        
        //Read the cache levels of the thread
        for (unsigned cache_index = 1; cache_index <= 3; ++cache_index) { // Iterate over 1, 2, and 3
            size_kb = (cache_index == 1)
                      ? _readIntegerFromFile(_getCacheLevelPath(cpu, 0) + "/size") + _readIntegerFromFile(_getCacheLevelPath(cpu, 1) + "/size")
                      : _readIntegerFromFile(_getCacheLevelPath(cpu, cache_index) + "/size");
            
            shared_cpus_str = _readStringFromFile(_getCacheLevelPath(cpu, cache_index) + "/shared_cpu_list");
            auto sharedCPUs = _parseCPUList(shared_cpus_str);
            if (cacheMap.find(cache_index) == cacheMap.end())
                cacheMap[cache_index] = std::map<std::vector<unsigned>, STLKR_Machine_CacheLevel*>();
            if (cacheMap[cache_index].find(sharedCPUs) == cacheMap[cache_index].end())
                cacheMap[cache_index][sharedCPUs] = new STLKR_Machine_CacheLevel(cache_index, size_kb, sharedCPUs);
            threadToCacheLevels[threadId][cache_index - 1] = cacheMap[cache_index][sharedCPUs];
        }
        _threads[threadId] = new STLKR_Machine_Thread(threadId,
                                                      _readIntegerFromFile(_getThreadPath(cpu) + "/cpufreq/cpuinfo_min_freq"),
                                                      _readIntegerFromFile(_getThreadPath(cpu) + "/cpufreq/cpuinfo_max_freq"),
                                                      new STLKR_Machine_SharedCache(threadToCacheLevels[threadId][0],
                                                                                    threadToCacheLevels[threadId][1],
                                                                                    threadToCacheLevels[threadId][2]));
        threadId++;
    }
    threadId = 0;
    coreTopologyId = 0;
    auto coreThreads = std::vector<STLKR_Machine_Thread*>();
    for (const auto& [coreId, threads] : physicalCoreToThreads) {
        for (auto iThread : threads){
            coreThreads.push_back(_threads[iThread]);
        }
        _physicalCores.emplace_back(new STLKR_Machine_Core(coreTopologyId, coreThreads));
        coreThreads.clear();
        coreTopologyId++;
    }

    std::cout << "Cache info read successfully!" << std::endl;
}



void STLKR_Machine_CPUTopologyLinux::print_processor_specs() const {
//    // Print Physical Cores and Logical Processors
//    std::cout << "Physical Cores and Logical Processors:\n";
//    for (const auto& core : core_info) {
//        std::cout << "Core " << core.first << ": ";
//        for (auto cpu : core.second) {
//            std::cout << cpu << " ";
//        }
//        std::cout << "\n";
//    }
//
//    // Print Clock Information
//    std::cout << "\nClock Information:\n";
//    for (const auto& clock : clock_info) {
//        std::cout << "Core " << clock.first << ": Min Frequency = " << clock.second.first << " kHz, Max Frequency = " << clock.second.second << " kHz\n";
//    }
//
//    // Print Cache Information
//    std::cout << "\nCache Information:\n";
//    for (const auto& core_cache : cache_info) {
//        std::cout << "Core " << core_cache.first << ":\n";
//        for (const auto& cache_level : core_cache.second) {
//            auto& cache = cache_level.second;
//            std::cout << "  L" << cache_level.first << " Cache: Size = " << cache.first / 1024 << " KB, Shared with CPUs = ";
//            for (auto cpu : cache.second) {
//                std::cout << cpu << " ";
//            }
//            std::cout << "\n";
//        }
//    }
}

unsigned STLKR_Machine_CPUTopologyLinux::_readIntegerFromFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + path);
    }
    unsigned value;
    file >> value;
    return value;
}

std::string STLKR_Machine_CPUTopologyLinux::_readStringFromFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + path);
    }
    std::string value;
    std::getline(file, value);
    return value;
}

// Helper function to parse CPU lists like "0-3,5,7-8"
std::vector<unsigned> STLKR_Machine_CPUTopologyLinux::_parseCPUList(const std::string& cpu_list) {
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

std::string STLKR_Machine_CPUTopologyLinux::_getThreadPath(unsigned int &threadId) const {
    return _cpuPath + "cpu" + std::to_string(threadId);
}

std::string STLKR_Machine_CPUTopologyLinux::_getCacheLevelPath(unsigned int threadId, unsigned int cacheIndex) const {
    return _cpuPath + "cpu" + std::to_string(threadId) + "/cache/index" + std::to_string(cacheIndex);
}



