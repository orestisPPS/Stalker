#include <unordered_set>
#include <list>
#include <utility>
#include <algorithm>
#include "CPUTopologyLinux.h"



CPUTopologyLinux::CPUTopologyLinux(std::string cpuPath){
    _cpuPath = std::move(cpuPath);
    readMachineCores();
    
}

CPUTopologyLinux::~CPUTopologyLinux() {
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

void CPUTopologyLinux::readMachineCores() {
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
    auto threadToCacheLevels = std::unordered_map<unsigned, CacheLevel*[3]>();
    for (unsigned iThread : onlineThreads) {
        //Read the core id of the thread
        coreTopologyId = _readIntegerFromFile(
                _getThreadPath(iThread) + "/topology/core_id");
        auto threadSiblings = _parseCPUList(_readStringFromFile(_getThreadPath(iThread) + "/topology/thread_siblings_list"));
        if (physicalCoreToThreads.find(coreTopologyId) == physicalCoreToThreads.end())
            physicalCoreToThreads[coreTopologyId] = std::move(threadSiblings);
        
        //Read the cache levels of the thread
        for (unsigned cache_index = 1; cache_index <= 3; ++cache_index) { // Iterate over 1, 2, and 3
            size_kb = (cache_index == 1)
                      ? _readIntegerFromFile(_getCacheLevelPath(iThread, 0) + "/size") + _readIntegerFromFile(_getCacheLevelPath(iThread, 1) + "/size")
                      : _readIntegerFromFile(_getCacheLevelPath(iThread, cache_index) + "/size");
            
            shared_cpus_str = _readStringFromFile(_getCacheLevelPath(iThread, cache_index) + "/shared_cpu_list");
            auto sharedCPUs = _parseCPUList(shared_cpus_str);
            if (cacheMap.find(cache_index) == cacheMap.end())
                cacheMap[cache_index] = std::map<std::vector<unsigned>, CacheLevel*>();
            if (cacheMap[cache_index].find(sharedCPUs) == cacheMap[cache_index].end()){
                cacheMap[cache_index][sharedCPUs] = new CacheLevel(cache_index, size_kb, sharedCPUs);
                _cacheLevels.push_back(cacheMap[cache_index][sharedCPUs]);
            }
            threadToCacheLevels[iThread][cache_index - 1] = cacheMap[cache_index][sharedCPUs];
        }
        auto sharedCache = new SharedCache(threadToCacheLevels[iThread][0],
                                                         threadToCacheLevels[iThread][1],
                                                         threadToCacheLevels[iThread][2]);
        _sharedCaches.push_back(sharedCache);
        _threads[iThread] = new Thread(iThread, coreTopologyId,
                                                      _readIntegerFromFile(_getThreadPath(iThread) + "/cpufreq/cpuinfo_min_freq"),
                                                      _readIntegerFromFile(_getThreadPath(iThread) + "/cpufreq/cpuinfo_max_freq"),
                                                      sharedCache);
    }
    _physicalCores = std::vector<Core*>();
    _physicalCores.reserve(physicalCoreToThreads.size());
    auto coreThreads = std::vector<Thread*>();
    for (const auto& [coreId, threads] : physicalCoreToThreads) {
        for (auto threadID : threads)
            coreThreads.push_back(_threads[threadID]);
        _physicalCores.emplace_back(new Core(coreId, coreThreads));
        coreThreads.clear();
    }
    //sort the cores by id
    std::sort(_physicalCores.begin(), _physicalCores.end(), [](const Core* a, const Core* b) {
        return a->getId() < b->getId();
    });
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Time to read the machine cores: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " μs" << std::endl;
    std::cout << "Cache info read successfully!" << std::endl;
}



void CPUTopologyLinux::print_processor_specs() const {
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

std::vector<Core*> CPUTopologyLinux::getPhysicalCores() const {
    return _physicalCores;
}

std::vector<Thread*> CPUTopologyLinux::getThreads() const {
    return _threads;
}

std::vector<CacheLevel*> CPUTopologyLinux::getCacheLevels() const {
    return _cacheLevels;
}

std::vector<SharedCache*> CPUTopologyLinux::getSharedCaches() const {
    return _sharedCaches;
}

unsigned CPUTopologyLinux::_readIntegerFromFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + path);
    }
    unsigned value;
    file >> value;
    return value;
}

std::string CPUTopologyLinux::_readStringFromFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + path);
    }
    std::string value;
    std::getline(file, value);
    return value;
}

// Helper function to parse CPU lists like "0-3,5,7-8"
std::vector<unsigned> CPUTopologyLinux::_parseCPUList(const std::string& cpu_list) {
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

std::string CPUTopologyLinux::_getThreadPath(unsigned int &threadId) const {
    return _cpuPath + "cpu" + std::to_string(threadId);
}

std::string CPUTopologyLinux::_getCacheLevelPath(unsigned int threadId, unsigned int cacheIndex) const {
    return _cpuPath + "cpu" + std::to_string(threadId) + "/cache/index" + std::to_string(cacheIndex);
}



