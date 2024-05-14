//
// Created by hal9000 on 11/21/23.
//

#include <filesystem>
#include <vector>
#include <algorithm>
#include "Logs.h"


Logs::Logs(std::string logName) :   _logName(std::move(logName)),
                                    _currentTimers(make_unique<unordered_map<string, Timer>>()),
                                    _singleObservationData(make_unique<unordered_map<string, list<double>>>()),
                                    _multipleObservationData(make_unique<unordered_map<string, list<list<double>>>>()),
                                    _singleObservationTimers(make_unique<unordered_map<string, list<chrono::duration<double>>>>()),
                                    _multipleObservationTimers(make_unique<unordered_map<string, list<list<chrono::duration<double>>>>>()),
                                    _parameters(make_unique<unordered_map<string, string>>()),
                                    _comments(make_unique<list<string>>()),
                                    _singleObservationTimerIndex(unordered_map<string, short>()),
                                    _multipleObservationTimerIndex(unordered_map<string, short>()),
                                    _singleObservationDataIndex(unordered_map<string, short>()),
                                    _multipleObservationDataIndex(unordered_map<string, short>()),
                                    _parametersIndex(unordered_map<string, short>()){}

void Logs::addComment(string comment) {
    _comments->push_back(std::move(comment));
}


void Logs::addParameter(const string &parameterName, double value) {
    _parameters->insert(make_pair(parameterName, to_string(value)));
    _parametersIndex.emplace(parameterName, _parameters->size());
}

void Logs::addParameter(const string &parameterName, const string &value) {
    _parameters->insert(make_pair(parameterName, value));
    _parametersIndex.emplace(parameterName, _parameters->size());
}

void Logs::startSingleObservationTimer(const string &logName, STLKR_TimeUnit unit) {

    if (_currentTimers->find(logName) == _currentTimers->end())
        _currentTimers->insert(make_pair(logName, Timer(unit)));
    _currentTimers->at(logName) = Timer(unit);
    _currentTimers->at(logName).start();
}

void Logs::stopSingleObservationTimer(const std::string &logName) {
    if (_currentTimers->find(logName) == _currentTimers->end())
        throw std::runtime_error("Logs::stopSingleObservationTimer: Timer " + logName + " does not exist.");

    _currentTimers->at(logName).stop();
    if (_singleObservationTimers->find(logName) == _singleObservationTimers->end())
        _singleObservationTimers->emplace(logName, list<chrono::duration<double>>());
    
    _singleObservationTimers->at(logName).push_back(_currentTimers->at(logName).duration());
    _singleObservationTimerIndex.emplace(logName, _singleObservationTimers->size());
}

void Logs::startMultipleObservationsTimer(const std::string &logName, STLKR_TimeUnit unit) {
    auto timer = Timer(unit);
    if (_currentTimers->find(logName) == _currentTimers->end())
        _currentTimers->insert(make_pair(logName, timer));
    _currentTimers->at(logName).start();
}

void Logs::stopMultipleObservationsTimer(const std::string &logName) {
    if (_currentTimers->find(logName) == _currentTimers->end())
        throw std::runtime_error("Logs::stopMultipleObservationsTimer: Timer " + logName + " does not exist.");

    _currentTimers->at(logName).stop();

    if (_multipleObservationTimers->find(logName) == _multipleObservationTimers->end())
        _multipleObservationTimers->emplace(logName, list<list<chrono::duration<double>>>());
    
    if (_multipleObservationTimers->at(logName).empty()) {
        _multipleObservationTimers->at(logName).emplace_back();
    }
    _multipleObservationTimers->at(logName).back().push_back(_currentTimers->at(logName).duration());
    _multipleObservationTimerIndex.emplace(logName, _multipleObservationTimers->size());
}


void Logs::storeAndResetCurrentLogs() {
    for (auto &timerPair : *_multipleObservationTimers) {
        timerPair.second.emplace_back();
    }
    _currentTimers->clear();
    
    for (auto &data: *_multipleObservationData) {
        data.second.emplace_back();
    }
}

void Logs::setSingleObservationLogData(const std::string &logName, double value) {
    if (_singleObservationData->find(logName) == _singleObservationData->end())
        _singleObservationData->emplace(logName, list<double>());
    _singleObservationData->at(logName).push_back(value);
    
    _singleObservationDataIndex.emplace(logName, _singleObservationData->size());
}

void Logs::setMultipleObservationsLogData(const std::string &logName, double value) {
    // Check if logName exists in the map
    if (_multipleObservationData->find(logName) == _multipleObservationData->end()) {
        _multipleObservationData->emplace(logName, list<list<double>>{list<double>()});
    }
    _multipleObservationData->at(logName).back().push_back(value);
    
    _singleObservationDataIndex.emplace(logName, _multipleObservationData->size());
}

void Logs::setMultipleObservationsLogData(const std::string &logName, const list<double> &values) {
    if (_multipleObservationData->find(logName) == _multipleObservationData->end()) {
        _multipleObservationData->emplace(logName, list<list<double>>());
    }
    // Add the new vector of values to the list
    _multipleObservationData->at(logName).push_back(values);
    
    _multipleObservationDataIndex.emplace(logName, _multipleObservationData->size());
}


void Logs::exportToCSV(const string &filePath, const string &fileName) {
    // Get current time
    auto now = std::chrono::system_clock::now();
    auto now_as_time_t = std::chrono::system_clock::to_time_t(now);
    auto now_ms = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()) % 1000;
    std::tm now_tm = *std::localtime(&now_as_time_t);

    // Create a timestamp string with milliseconds
    std::ostringstream timestamp;
    timestamp << std::put_time(&now_tm, "%d%m%Y_%H%M%S_") << std::setfill('0') << std::setw(3) << now_ms.count();
    // Create a unique filename with timestamp
    std::string filename = filePath + "/" + fileName + "_" + timestamp.str() + ".csv";
    if (std::filesystem::exists(filename)) {
        throw std::runtime_error("File already exists: " + filename);
    }
    std::ofstream file(filename);

    // Check if file is opened successfully
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open file: " + filename);
    }
    file << std::scientific << std::setprecision(15);
    
    // Adding comments
    if (!_comments->empty()) {
        file << "#Region: Comments" << std::endl;
        for (const auto &comment : *_comments) {
            file << "# " << comment << std::endl;
        }
        file << std::endl;
        file.flush();
    }
    
    // Adding parameters
    if (!_parameters->empty()) {
        file << "#Region: Parameters" << std::endl;
        auto listOfData = list<tuple<string, string, unsigned>>();
        for (const auto &pair : *_parameters) {
            listOfData.emplace_back(pair.first, pair.second, _parametersIndex.at(pair.first));
        }
        listOfData.sort([](const tuple<string, string, unsigned> &a, const tuple<string, string, unsigned> &b) {
            return std::get<2>(a) < std::get<2>(b);
        });
        for (const auto &data : listOfData) {
            file << "Parameter:" << std::get<0>(data) << std::endl;
            file << std::get<1>(data) << std::endl;
            file << std::endl;
            file.flush();
        }
        file << std::endl;
        file.flush();
    }
    
    // Adding single observation Data
    if (!_singleObservationData->empty()) {
        list<tuple<string, list<double>, unsigned>> listOfData = {};
        for (const auto &pair : *_singleObservationData) {
            listOfData.emplace_back(pair.first, pair.second, _singleObservationDataIndex.at(pair.first));
        }
        listOfData.sort([](const tuple<string, list<double>, unsigned> &a, const tuple<string, list<double>, unsigned> &b) {
            return std::get<2>(a) < std::get<2>(b);
        });
        
        
        file << "#Region: Single Observation Data" << std::endl;
        for (const auto &data : listOfData) {
            file << "LogEntry:" << std::get<0>(data) << std::endl;
            for (const auto &value : std::get<1>(data)) {
                file << value << std::endl;
            }
            file << std::endl;
            file.flush();
        }
    }

    unsigned index = 0;
    unsigned logIndex = 0;
    size_t maxLogSize;

    // Adding multiple observation Data
    if (!_multipleObservationData->empty()) {
        list<tuple<string, list<list<double>>, unsigned>> listOfData = {};
        for (const auto &pair : *_multipleObservationData) {
            listOfData.emplace_back(pair.first, pair.second, _multipleObservationDataIndex.at(pair.first));
        }
        listOfData.sort([](const tuple<string, list<list<double>>, unsigned> &a, const tuple<string, list<list<double>>, unsigned> &b) {
            return std::get<2>(a) < std::get<2>(b);
        });
        
        file << "#Region: Multiple Observation Data" << std::endl;

        for (const auto& pair : listOfData) {
            list<list<double>> listOfLists = std::get<1>(pair);
            logIndex = 0;
            maxLogSize = 0;
            for (const auto &listInList: listOfLists) {
                maxLogSize = std::max(maxLogSize, listInList.size());
            }
            auto dataMatrix = new double[maxLogSize * listOfLists.size()];
            for (const auto &listInList: listOfLists) {
                for (const auto &value: listInList) {
                    dataMatrix[index * listOfLists.size() + logIndex] = value;
                    ++index;
                }
                ++logIndex;
                index = 0;
            }
            file << "LogEntry:" << std::get<0>(pair) << std::endl;
            for (size_t i = 0; i < maxLogSize; ++i) {
                for (size_t j = 0; j < listOfLists.size(); ++j) {
                    file << dataMatrix[i * listOfLists.size() + j];
                    if (j != listOfLists.size() - 1) {
                        file << ",";
                    }
                }
                file << std::endl;
            }
            file << std::endl;
            file.flush();
            delete[] dataMatrix;
            dataMatrix = nullptr;
        }
    }
    
    // Adding single observation timers
    if (!_singleObservationTimers->empty()) {
        file << "#Region: Single Observation Timers" << std::endl;
        list<tuple<string, list<chrono::duration<double>>, unsigned>> listOfData = {};
        for (const auto &pair : *_singleObservationTimers) {
            listOfData.emplace_back(pair.first, pair.second, _singleObservationTimerIndex.at(pair.first));
        }
        listOfData.sort([](const tuple<string, list<chrono::duration<double>>, unsigned> &a, const tuple<string, list<chrono::duration<double>>, unsigned> &b) {
            return std::get<2>(a) < std::get<2>(b);
        });
        
        
        for (const auto &timerPair : listOfData) {
            file << "Timer:" << std::get<0>(timerPair) << std::endl;
            for (const auto &duration : std::get<1>(timerPair)) {
                file << duration.count() << std::endl;
            }
            file << std::endl;
            file.flush();
        }
    }
    // Adding multiple observation timers
    if (!_multipleObservationTimers->empty()) {
        file << "#Region: Multiple Observation Timers" << std::endl;
        list<tuple<string, list<list<chrono::duration<double>>>, unsigned>> listOfData = {};
        for (const auto &pair : *_multipleObservationTimers) {
            listOfData.emplace_back(pair.first, pair.second, _multipleObservationTimerIndex.at(pair.first));
        }
        listOfData.sort([](const tuple<string, list<list<chrono::duration<double>>>, unsigned> &a, const tuple<string, list<list<chrono::duration<double>>>, unsigned> &b) {
            return std::get<2>(a) < std::get<2>(b);
        });
        
        for (const auto &pair: listOfData) {
            list<list<chrono::duration<double>>> listOfLists = std::get<1>(pair);
            logIndex = 0;
            maxLogSize = 0;
            for (const auto &listInList: listOfLists) {
                maxLogSize = std::max(maxLogSize, listInList.size());
            }
            auto dataMatrix = new double[maxLogSize * listOfLists.size()];
            for (const auto &listInList: listOfLists) {
                for (const auto &value: listInList) {
                    dataMatrix[index * listOfLists.size() + logIndex] = value.count();
                    ++index;
                }
                ++logIndex;
                index = 0;
            }
            file << "Timer:" << std::get<0>(pair) << std::endl;
            for (size_t i = 0; i < maxLogSize; ++i) {
                for (size_t j = 0; j < listOfLists.size(); ++j) {
                    file << dataMatrix[i * listOfLists.size() + j];
                    if (j != listOfLists.size() - 1) {
                        file << ",";
                    }
                }
                file << std::endl;
            }
            file << std::endl;
            file.flush();
            delete[] dataMatrix;
            dataMatrix = nullptr;
        }
    }
    file.flush();
    file.close();
}

void Logs::clearAllLogs() {
    _currentTimers->clear();
    _singleObservationData->clear();
    _multipleObservationData->clear();
    _singleObservationTimers->clear();
    _multipleObservationTimers->clear();
    _parameters->clear();
    _comments->clear();
}




