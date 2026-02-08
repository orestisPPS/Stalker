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
// Created by hal9000 on 11/21/23.
//
#pragma once

#include <fstream>
#include <algorithm>
#include <list>
#include <filesystem>
#include <random>
#include <sstream>
#include <Stalker/Core/Units.h>
#include <Stalker/Utility/Stopwatch.h>
#include <Stalker/Utility/Printers.h>

using namespace std;

namespace Stalker::Utility{

struct PlotConfig{
    string title;
    string xLabel;
    string yLabel;
    string outputFileName;
    bool legend = true;
    bool grid = true;
    bool box = true;
    int width = 800;
    int height = 600;
    int pointSize = 1;
    int lineWidth = 1;
    int bins = 10;
};

class Logs {

    using Timer = Stalker::Utility::Timer;
    using TimeUnit = Stalker::Core::TimeUnit;
    using Clock = std::chrono::system_clock;

    
    public:
    Logs(const string &name = "") : _name(name) {}

    PlotConfig plot;

    struct Measurement {
        string name;
        double value;
    };

    struct MeasurementSet {
        struct Entry {
            string name;
            std::vector<double> values;
            std::vector<std::string> compareOver;
        };
        string name;
        bool plot = true;
        std::vector<Entry> entries;
    };

    void addComment(const string &comment) {
        _comments.push_back(comment);
    }

    template<typename T>
    void addParameter(const string &parameterName, T value) {

        static_assert(std::is_arithmetic_v<T> || std::is_convertible_v<T, std::string>, "addParameter only supports arithmetic types or types convertible to std::string.");

        std::string valueStr;
        if constexpr (std::is_arithmetic_v<T>) {
            valueStr = std::to_string(value);
        } else if constexpr (std::is_convertible_v<T, std::string>) {
            valueStr = static_cast<std::string>(value);
        } else {
            std::stringstream ss;
            ss << value;
            valueStr = ss.str();
        }

        auto it = std::find_if(_parameters.begin(), _parameters.end(),
            [&](const std::pair<std::string, std::string>& p) { return p.first == parameterName; });

        if (it != _parameters.end()) {
            printWarning("Parameter '" + parameterName + "' already exists. Overwriting its value.");
            it->second = valueStr;
        } else {
            _parameters.push_back(std::make_pair(parameterName, valueStr));
        }
    }

    void addTimer(const string &timerName, const Timer &timer) {
        auto it = std::find_if(_timers.begin(), _timers.end(),
            [&](const std::pair<std::string, Timer>& p) { return p.first == timerName; });

        if (it != _timers.end()) {
            printWarning("Warning: Timer '" + timerName + "' already exists. Overwriting its value.");
            it->second = timer;
        } else {
            _timers.push_back(std::make_pair(timerName, timer));
        }
    }

    template<typename T>
    void addMeasurement(const string &dataName, const T &data) {
        static_assert(std::is_arithmetic_v<T>, "addMeasurement only supports arithmetic types.");
        auto it = std::find_if(_measurements.begin(), _measurements.end(),
            [&](const Measurement& m) { return m.name == dataName; });

        if (it != _measurements.end()) {
            printWarning("Measurement '" + dataName + "' already exists. Overwriting its value.");
            it->value = static_cast<double>(data);
        } else {
            _measurements.push_back(Measurement{dataName, static_cast<double>(data)});
        }
    }

    double getMeasurementValue(const string &dataName) const {
        auto it = std::find_if(_measurements.begin(), _measurements.end(),
            [&](const Measurement& m) { return m.name == dataName; });

        if (it != _measurements.end()) {
            return it->value;
        } else {
            throw std::runtime_error("Measurement '" + dataName + "' not found.");
        }
    }

    void addMeasurementSet(const std::string &setName, bool plot = true) {
        auto it = std::find_if(_measurementSets.begin(), _measurementSets.end(),
            [&](const MeasurementSet& m) { return m.name == setName; });

        if (it != _measurementSets.end()) {
            // Set already exists, just update plot preference if needed, but don't wipe data
            it->plot = plot;
            return;
        }

        MeasurementSet newSet;
        newSet.name = setName;
        newSet.plot = plot;
        _measurementSets.push_back(std::move(newSet));
    }

    template<typename T>
    void addMeasurementToSet(const string &setName,
                             const string &dataName,
                             const T &data,
                             const std::vector<std::string>& compareOver = {}) {

        static_assert(std::is_arithmetic_v<T>, "addMeasurementToSet only supports arithmetic types.");
        auto it = std::find_if(_measurementSets.begin(), _measurementSets.end(),
            [&](const MeasurementSet& m) { return m.name == setName; });
        if (it == _measurementSets.end()) {
            MeasurementSet newSet;
            newSet.name = setName;
            MeasurementSet::Entry e;
            e.name = dataName;
            e.values.emplace_back(static_cast<double>(data));
            e.compareOver = compareOver;
            newSet.entries.emplace_back(std::move(e));
            _measurementSets.push_back(std::move(newSet));
            return;
        }

        // Find existing measurement entry inside the set
        auto &entries = it->entries;
        auto mit = std::find_if(entries.begin(), entries.end(), [&](const MeasurementSet::Entry& e){
            return e.name == dataName;
        });
        if (mit != entries.end()) {
            mit->values.push_back(static_cast<double>(data));
        } else {
            MeasurementSet::Entry e;
            e.name = dataName;
            e.values.emplace_back(static_cast<double>(data));
            e.compareOver = compareOver;
            entries.emplace_back(std::move(e));
        }
    }

    void addMeasurementToSet(const string &setName, const string &dataName, const std::vector<std::string>& compareOver = {}) {
        auto it = std::find_if(_measurementSets.begin(), _measurementSets.end(),
            [&](const MeasurementSet& m) { return m.name == setName; });
        if (it == _measurementSets.end()) {
            MeasurementSet newSet;
            newSet.name = setName;
            MeasurementSet::Entry e;
            e.name = dataName;
            e.compareOver = compareOver;
            newSet.entries.emplace_back(std::move(e));
            _measurementSets.push_back(std::move(newSet));
            return;
        }

        // Find existing measurement entry inside the set
        auto &entries = it->entries;
        auto mit = std::find_if(entries.begin(), entries.end(), [&](const MeasurementSet::Entry& e){
            return e.name == dataName;
        });
        if (mit == entries.end()) {
            MeasurementSet::Entry e;
            e.name = dataName;
            entries.emplace_back(std::move(e));
        }
    }

    MeasurementSet getMeasurementSet(const string &setName) const {
        auto it = std::find_if(_measurementSets.begin(), _measurementSets.end(),
            [&](const MeasurementSet& m) { return m.name == setName; });
        if (it != _measurementSets.end()) {
            return *it;
        } else {
            throw std::runtime_error("Measurement set '" + setName + "' not found.");
        }
    }

    std::vector<double> getMeasurementValues(const string &setName, const string &measurementName) {
        auto it = std::find_if(_measurementSets.begin(), _measurementSets.end(),
            [&](const MeasurementSet& m) { return m.name == setName; });
        if (it == _measurementSets.end()) {
            throw std::runtime_error("Measurement set '" + setName + "' not found.");
        }
        auto &entries = it->entries;
        auto mit = std::find_if(entries.begin(), entries.end(), [&](const MeasurementSet::Entry& e){
            return e.name == measurementName;
        });
        if (mit == entries.end()) {
            throw std::runtime_error("Measurement '" + measurementName + "' not found in set '" + setName + "'.");
        }
        return mit->values;
    }


    void addStopwatch(const Stopwatch &stopwatch) {
        auto it = std::find_if(_stopwatches.begin(), _stopwatches.end(),
            [&](const Stopwatch& sw) { return sw.getName() == stopwatch.getName(); });
        if (it != _stopwatches.end()) {
            printWarning("Warning: Stopwatch '" + stopwatch.getName() + "' already exists. Overwriting its value.");
            *it = stopwatch;
        } else {
            _stopwatches.push_back(stopwatch);
        }
    }

    Stopwatch& getStopwatch(const string &name) {
        auto it = std::find_if(_stopwatches.begin(), _stopwatches.end(),
            [&](const Stopwatch& sw) { return sw.getName() == name; });
        if (it != _stopwatches.end()) {
            return *it;
        } else {
            throw std::runtime_error("Stopwatch '" + name + "' not found.");
        }
    }

    Stopwatch& getOrCreateStopwatch(const string &name) {
        auto it = std::find_if(_stopwatches.begin(), _stopwatches.end(),
            [&](const Stopwatch& sw) { return sw.getName() == name; });
        if (it != _stopwatches.end()) {
            return *it;
        } else {
            _stopwatches.emplace_back(name);
            return _stopwatches.back();
        }
    }

    // Export current logs to JSON next to CSV functionality.
    // Follows identical initialization, naming conventions and filename scheme as exportToCSV.
    // New API: directory path + base filename (without extension)
    // precision: numeric precision for floating-point in JSON (default 7)
    // unit: time unit for timers/stopwatches values (default seconds)
    void exportToJSON(const std::string& directoryPath, const std::string& baseFileName, int precision = 7, TimeUnit unit = TimeUnit::seconds) {
        
        std::filesystem::path dir(directoryPath);
        if (!dir.empty() && !std::filesystem::exists(dir)) {
            if (!std::filesystem::create_directories(dir)) {
                throw std::runtime_error("Unable to create directory: " + dir.string());
            }
        }

        // Generate timestamp and UUID like CSV
        auto now_as_time_t = std::chrono::system_clock::to_time_t(Clock::now());
        auto now_ms = std::chrono::duration_cast<std::chrono::microseconds>(Clock::now().time_since_epoch()) % 1000;
        std::tm now_tm = *std::localtime(&now_as_time_t);
        std::ostringstream timestamp;
        timestamp << std::put_time(&now_tm, "%d%m%Y_%H%M%S_") << std::setfill('0') << std::setw(3) << now_ms.count();
        std::string stem = baseFileName;
        if (stem.empty()) stem = _name.empty() ? std::string("logs") : _name;
        std::filesystem::path outPath = dir / (stem + "_" + timestamp.str() + "_" + _generateUUID() + ".json");

        std::ofstream file(outPath.string());
        if (!file.is_open()) {
            throw std::runtime_error("Unable to open file: " + outPath.string());
        }

    if (precision < 0) precision = 0;
    if (precision > 20) precision = 20; // clamp to reasonable JSON precision
    file << std::scientific << std::setprecision(precision);

        // JSON header
        file << "{\n";

        // _parameters as an object map
        file << "  \"_parameters\": {\n";
            for (size_t i = 0; i < _parameters.size(); ++i) {
                const auto &p = _parameters[i];
                file << "    \"" << _jsonEscape(p.first) << "\": \"" << _jsonEscape(p.second) << "\"";
                if (i + 1 < _parameters.size()) file << ",";
                file << "\n";
            }
            file << "  },\n";

        // _comments as array of strings
        file << "  \"_comments\": [\n";
            for (size_t i = 0; i < _comments.size(); ++i) {
                file << "    \"" << _jsonEscape(_comments[i]) << "\"";
                if (i + 1 < _comments.size()) file << ",";
                file << "\n";
            }
            file << "  ],\n";

        // _measurements as an object map
        file << "  \"_measurements\": {\n";
            for (size_t i = 0; i < _measurements.size(); ++i) {
                const auto &m = _measurements[i];
                file << "    \"" << _jsonEscape(m.name) << "\": " << m.value;
                if (i + 1 < _measurements.size()) file << ",";
                file << "\n";
            }
            file << "  },\n";

        // _measurementSets as an object map: setName -> { measurementName: { "values": [...], "compareOver": [..] }, ..., "_plot": bool }
        file << "  \"_measurementSets\": {\n";
        for (size_t i = 0; i < _measurementSets.size(); ++i) {
            const auto &ms = _measurementSets[i];
            file << "    \"" << _jsonEscape(ms.name) << "\": {\n";
            if (!ms.entries.empty()) {
            for (size_t j = 0; j < ms.entries.size(); ++j) {
                const auto &entry = ms.entries[j];
                file << "      \"" << _jsonEscape(entry.name) << "\": {\n";
                // values
                file << "        \"values\": [";
                for (size_t k = 0; k < entry.values.size(); ++k) {
                    file << entry.values[k];
                    if (k + 1 < entry.values.size()) file << ", ";
                }
                file << "],\n";
                // compareOver (string array)
                file << "        \"compareOver\": [";
                for (size_t k = 0; k < entry.compareOver.size(); ++k) {
                    file << "\"" << _jsonEscape(entry.compareOver[k]) << "\"";
                    if (k + 1 < entry.compareOver.size()) file << ", ";
                }
                file << "]\n";
                file << "      }";
                file << ",\n";
            }
            }
            // include plot flag for the set (so consumers can choose to plot or not)
            file << "      \"_plot\": " << (ms.plot ? "true" : "false") << "\n";
            file << "    }";
            if (i + 1 < _measurementSets.size()) file << ",";
            file << "\n";
        }
        file << "  },\n";

        // _timers as an object map
        file << "  \"_timers\": {\n";
        for (size_t i = 0; i < _timers.size(); ++i) {
            const auto &t = _timers[i];
            file << "    \"" << _jsonEscape(t.first) << "\": " << t.second.durationValue(unit);
            if (i + 1 < _timers.size()) file << ",";
            file << "\n";
        }
        file << "  },\n";

        // _stopwatches as an object map of objects: name -> { "values": [...], "compareOver": [..] }
        file << "  \"_stopwatches\": {\n";
        for (size_t i = 0; i < _stopwatches.size(); ++i) {
            const auto &sw = _stopwatches[i];
            file << "    \"" << _jsonEscape(sw.getName()) << "\": {\n";
            // values
            file << "      \"values\": [";
            auto measurements = sw.getAllMeasurementsValues(unit);
            for (size_t j = 0; j < measurements.size(); ++j) {
                file << measurements[j];
                if (j + 1 < measurements.size()) file << ", ";
            }
            file << "],\n";
            // compareOver from stopwatch tags (if any)
            file << "      \"compareOver\": [";
            const auto &tags = sw.getTags();
            for (size_t t = 0; t < tags.size(); ++t) {
                file << "\"" << _jsonEscape(tags[t]) << "\"";
                if (t + 1 < tags.size()) file << ", ";
            }
            file << "]\n";
            file << "    }";
            if (i + 1 < _stopwatches.size()) file << ",";
            file << "\n";
        }
        file << "  }\n";

        file << "}\n";
        file.flush();
        file.close();

        printSuccess("Logs exported to JSON: " + outPath.string() + ".");
    }

    // Backward-compatible wrapper: accept a full prefix path (directory + base prefix, no extension)
    // Allows optional precision and time unit overrides.
    void exportToJSON(const std::string& fullFilePathStr,
                      int precision = 17,
                      TimeUnit unit = TimeUnit::seconds) {
        std::filesystem::path p(fullFilePathStr);
        if (p.has_filename()) {
            exportToJSON(p.parent_path().string(), p.filename().string(), precision, unit);
        } else {
            exportToJSON(p.string(), std::string{}, precision, unit);
        }
    }

    void clear() {
        _parameters.clear();
        _comments.clear();
        _measurements.clear();
        _measurementSets.clear();
        _stopwatches.clear();
        _timers.clear();
    }
        
        
    private:

    std::vector<std::pair<std::string, std::string>> _parameters;
    std::vector<string> _comments;

    std::vector<Measurement> _measurements;
    std::vector<MeasurementSet> _measurementSets;

    std::vector<std::pair<std::string, Timer>> _timers;
    std::vector<Stopwatch> _stopwatches;
    
    string _name;

    string _generateUUID() {
        // Generate a short, filename-safe identifier.
        // 12 hex characters (~48 bits of randomness) is ample when combined with the timestamp in the filename.
        static std::random_device rd;
        static std::mt19937_64 gen(rd());
        static std::uniform_int_distribution<uint64_t> dis;

        uint64_t v = dis(gen) & 0xFFFFFFFFFFFFULL; // 48 bits
        std::ostringstream oss;
        oss << std::hex << std::nouppercase << std::setfill('0') << std::setw(12) << v;
        return oss.str();
    }

    // Minimal JSON string escaper (quotes, backslashes, control chars)
    static std::string _jsonEscape(const std::string& s) {
        std::string out;
        out.reserve(s.size() + 8);
        for (unsigned char c : s) {
            switch (c) {
                case '"': out += "\\\""; break;
                case '\\': out += "\\\\"; break;
                case '\b': out += "\\b"; break;
                case '\f': out += "\\f"; break;
                case '\n': out += "\\n"; break;
                case '\r': out += "\\r"; break;
                case '\t': out += "\\t"; break;
                default:
                    if (c < 0x20) {
                        std::ostringstream oss;
                        oss << "\\u" << std::hex << std::setw(4) << std::setfill('0') << int(c);
                        out += oss.str();
                    } else {
                        out += static_cast<char>(c);
                    }
            }
        }
        return out;
    }

    };
} // namespace Stalker::Utility