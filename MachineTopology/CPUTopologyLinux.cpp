#include <unordered_set>
#include <list>
#include <utility>
#include <algorithm>
#include "CPUTopologyLinux.h"



CPUTopologyLinux::CPUTopologyLinux(std::string cpuPath){
    _cpuPath = std::move(cpuPath);
    _readMachineTopology();
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

void CPUTopologyLinux::_readMachineTopology() {
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
            type = _readStringFromFile(_getCacheLevelPath(iThread, cache_index) + "/type");
            shared_cpus_str = _readStringFromFile(_getCacheLevelPath(iThread, cache_index) + "/shared_cpu_list");
            auto sharedCPUs = _parseCPUList(shared_cpus_str);
            if (cacheMap.find(cache_index) == cacheMap.end())
                cacheMap[cache_index] = std::map<std::vector<unsigned>, CacheLevel*>();
            unsigned level = 0;
            if (cacheMap[cache_index].find(sharedCPUs) == cacheMap[cache_index].end()){
                level = (cache_index == 0 || cache_index == 1) ? 1 : cache_index;
                cacheMap[cache_index][sharedCPUs] = new CacheLevel(level, size_kb, sharedCPUs, _getCacheLevelType(type));
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
        core->setHyperThreading(true);
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



void CPUTopologyLinux::print_processor_specs() const {
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
            std::cout << "Shared Cache: L1_Data Size = " << sharedCache->getCacheLevel1Data()->getSize() << " KB, "
                      << "L1_Instruction Size = " << sharedCache->getCacheLevel1Instructions()->getSize() << " KB, "
                      << "L2 Size = " << sharedCache->getCacheLevel2()->getSize() << " KB, "
                      << "L3 Size = " << sharedCache->getCacheLevel3()->getSize() << " KB\n";
        }
    }

    std::cout << "\n";
}


const std::vector<Core*> &CPUTopologyLinux::getCores(unsigned numCores) const {
    return _physicalCores;
}

const std::vector<Thread *> &CPUTopologyLinux::getThreads() const {
    return _threads;
}

const std::vector<CacheLevel*> &CPUTopologyLinux::getCacheLevels() const {
    return _cacheLevels;
}

const std::vector<SharedCache*> &CPUTopologyLinux::getSharedCaches() const {
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

CacheLevelType CPUTopologyLinux::_getCacheLevelType(const std::string &type) const {
    switch (type[0]) {
        case 'I':
            return CacheLevelType::INSTRUCTIONS;
        case 'D':
            return CacheLevelType::DATA;
        case 'U':
            return CacheLevelType::UNIFIED;
        default:
            throw std::runtime_error("Invalid cache type: " + type);
    }
}



