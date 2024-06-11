#include <unordered_set>
#include "STLKR_Thread_LinuxProcessorSpecsReader.h"

unsigned STLKR_Thread_LinuxProcessorSpecsReader::read_integer_from_file(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + path);
    }
    unsigned value;
    file >> value;
    return value;
}

std::string STLKR_Thread_LinuxProcessorSpecsReader::read_string_from_file(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + path);
    }
    std::string value;
    std::getline(file, value);
    return value;
}

// Helper function to parse CPU lists like "0-3,5,7-8"
std::vector<unsigned> STLKR_Thread_LinuxProcessorSpecsReader::parse_cpu_list(const std::string& cpu_list) {
    std::vector<unsigned> cpus;
    std::stringstream ss(cpu_list);
    std::string range;
    while (std::getline(ss, range, ',')) {
        size_t dash = range.find('-');
        if (dash != std::string::npos) {
            unsigned start = std::stoi(range.substr(0, dash));
            unsigned end = std::stoi(range.substr(dash + 1));
            for (unsigned i = start; i <= end; ++i) {
                cpus.push_back(i);
            }
        } else {
            cpus.push_back(std::stoi(range));
        }
    }
    return cpus;
}

STLKR_Thread_LinuxProcessorSpecsReader::STLKR_Thread_LinuxProcessorSpecsReader()
        : num_physical_cores(0), num_logical_processors(0) {}

void STLKR_Thread_LinuxProcessorSpecsReader::initialize() {
    auto startTime = std::chrono::high_resolution_clock::now();
    read_cpu_info();
    read_cache_info();
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
    std::cout << "Processor specs read in " << duration << " μs" << std::endl;
}

void STLKR_Thread_LinuxProcessorSpecsReader::read_cpu_info() {
    std::string online_path = "/sys/devices/system/cpu/online";
    std::string online_cpus = read_string_from_file(online_path);
    std::vector<unsigned> logical_cpus = parse_cpu_list(online_cpus);

    num_logical_processors = logical_cpus.size();
    std::unordered_map<unsigned, std::vector<unsigned>> temp_core_info;

    // Temporary set to keep track of already processed logical CPUs
    std::unordered_set<unsigned> processed_cores;

    for (unsigned cpu : logical_cpus) {
        std::string core_id_path = "/sys/devices/system/cpu/cpu" + std::to_string(cpu) + "/topology/core_id";
        unsigned core_id = read_integer_from_file(core_id_path);

        if (processed_cores.find(cpu) != processed_cores.end()) {
            continue;
        }

        // Find all siblings (hyper-threaded or eco threads) that share the same core_id
        std::vector<unsigned> siblings;
        for (unsigned sibling_cpu : logical_cpus) {
            std::string sibling_core_id_path = "/sys/devices/system/cpu/cpu" + std::to_string(sibling_cpu) + "/topology/core_id";
            unsigned sibling_core_id = read_integer_from_file(sibling_core_id_path);

            if (sibling_core_id == core_id) {
                siblings.push_back(sibling_cpu);
                processed_cores.insert(sibling_cpu);
            }
        }

        temp_core_info[core_id] = siblings;

        // Read clock frequencies
        std::string min_freq_path = "/sys/devices/system/cpu/cpu" + std::to_string(cpu) + "/cpufreq/cpuinfo_min_freq";
        std::string max_freq_path = "/sys/devices/system/cpu/cpu" + std::to_string(cpu) + "/cpufreq/cpuinfo_max_freq";

        unsigned min_freq = read_integer_from_file(min_freq_path);
        unsigned max_freq = read_integer_from_file(max_freq_path);

        clock_info[core_id] = {min_freq, max_freq};
    }

    // Assign a new sequential physical core ID for each unique core_id found
    unsigned physical_core_id = 0;
    for (const auto& [core_id, logical_cpus] : temp_core_info) {
        core_info[physical_core_id++] = logical_cpus;
    }

    num_physical_cores = core_info.size();
}


void STLKR_Thread_LinuxProcessorSpecsReader::read_cache_info() {
    unsigned int level, size_kb, size_bytes;
    std::string type, shared_cpus_str;
    std::vector<unsigned> shared_cpus;
    for (const auto& [core_id, cpus] : core_info) {
        for (unsigned cpu : cpus) {
            std::unordered_map<unsigned , std::pair<unsigned , std::vector<unsigned >>> core_cache_info;
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


std::unordered_map<unsigned, std::vector<unsigned>> STLKR_Thread_LinuxProcessorSpecsReader::get_physical_cores() const {
    return core_info;
}

std::unordered_map<unsigned, std::pair<unsigned, unsigned>> STLKR_Thread_LinuxProcessorSpecsReader::get_clock_info() const {
    return clock_info;
}

std::unordered_map<unsigned, std::unordered_map<unsigned, std::pair<unsigned , std::vector<unsigned>>>> STLKR_Thread_LinuxProcessorSpecsReader::get_cache_info() const {
    return cache_info;
}

void STLKR_Thread_LinuxProcessorSpecsReader::print_processor_specs() const {
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


