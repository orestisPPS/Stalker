//
// Created by hal9000 on 11/21/23.
//

#ifndef UNTITLED_LOGS_H
#define UNTITLED_LOGS_H
#include <iostream>
#include <memory>
#include <fstream>
#include <iomanip>
#include <sstream>
#include "Timer.h"
#include <string>
#include <list>
#include <unordered_map>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <thread>
#include <random>
using namespace std;

class Logs {
    
    public:
        explicit Logs(string logName = "");
        
        void addComment(string comment);
        
        void addParameter(const string &parameterName, double value);
        
        void addParameter(const string &parameterName, const string &value);
        
        //==========================================Timers==========================================
        
        void startSingleObservationTimer(const string &logName, STLKR_TimeUnit unit = STLKR_TimeUnit::seconds);
        
        void stopSingleObservationTimer(const string &logName, STLKR_TimeUnit unit);
        
        void startMultipleObservationsTimer(const string &logNameSTLKR_TimeUnit, STLKR_TimeUnit unit = STLKR_TimeUnit::seconds);
        
        void stopMultipleObservationsTimer(const string &logName);
        
        //==========================================Data==========================================
        
        void setSingleObservationLogData(const string &logName, double value);
        
        void setMultipleObservationsLogData(const string &logName, double value);
        
        void setMultipleObservationsLogData(const string &logName, const list<double> &values);
        
        void storeAndResetCurrentLogs();
        
        void clearAllLogs();
        
        void storeAndResetAllLogs();
        
        void exportToCSV(const string &filePath, const string &fileName);
        
        
    private:

        unique_ptr<unordered_map<string, Timer>> _currentTimers;
        unique_ptr<unordered_map<string, list<chrono::duration<double>>>> _singleObservationTimers;
        unique_ptr<unordered_map<string, list<list<chrono::duration<double>>>>> _multipleObservationTimers;
        unique_ptr<unordered_map<string, list<double>>> _singleObservationData;
        unique_ptr<unordered_map<string, list<list<double>>>> _multipleObservationData;
        
        unordered_map<string, short> _singleObservationTimerIndex;
        unordered_map<string, short> _multipleObservationTimerIndex;
        unordered_map<string, short> _singleObservationDataIndex;
        unordered_map<string, short> _multipleObservationDataIndex;
        unordered_map<string, short> _parametersIndex;
        
        unique_ptr<unordered_map<string, string>> _parameters;
        unique_ptr<list<string>> _comments;
            
        string _logName;

        static string _generateUUID();
};


#endif //UNTITLED_LOGS_H