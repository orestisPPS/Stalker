/*
 * Copyright 2024-2026 Papas Christoforos Orestis
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

//
// Created by hal9000 on 8/24/24.
//
#pragma once

#include <string>
#include <stdexcept>
#include <vector>
#include <fstream>
#include <sstream>

namespace Stalker::Utility {

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

} // namespace Stalker::Utility

