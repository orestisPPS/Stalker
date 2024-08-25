//
// Created by hal9000 on 8/24/24.
//

#ifndef STALKER_READERS_H
#define STALKER_READERS_H

#include <string>
#include <stdexcept>
#include <vector>
#include <fstream>
#include <sstream>

class Readers{
public:
    static inline unsigned readIntegerFromFile(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + path);
        }
        unsigned value;
        file >> value;
        return value;
    }

    static inline std::string readStringFromFile(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + path);
        }
        std::string value;
        std::getline(file, value);
        return value;
    }

    static inline std::vector<unsigned> parseCPUList(const std::string& cpu_list) {
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
};
#endif //STALKER_READERS_H
