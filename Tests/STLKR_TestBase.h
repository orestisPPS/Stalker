//
// Created by hal9000 on 4/29/24.
//

#ifndef STALKER_STLKR_TESTBASE_H
#define STALKER_STLKR_TESTBASE_H

#include <utility>
#include <string>
#include <iostream>
#include <unordered_map>
#include <filesystem>
#include <limits.h>
#include "TestUtility.h"
#include "../Utility/Linspace.h"
#include "../Utility/MemoryManagement.h"
#include "../Utility/Logs.h"
#include "../Threading/CPUTopology/CPU_Manager.h"


namespace STLKR_Tests {

    class STLKR_TestBase{

    public:
        virtual void runTest() = 0;
        
        void runTest(unsigned int iterations);

        const std::string& getPath(const std::string& pathName);

        void setPath(const std::string& pathName, const std::string& path);

    protected:
        explicit STLKR_TestBase(std::string testName);

        CPU_Manager _manager;

        Logs _logs;
        
        unsigned _caseCounter = 0;

        std::string _testName;

        std::unordered_map<std::string, std::string> _paths;
        
        static void printSuccess();

        static void printFailure();
            
        void resetCaseCounting();
            
        static void printTestCaseResult(bool success, const std::string &testNumber, const std::string &testName);

        void printTestCaseResult(bool success, const std::string& name);
    };

} // STLKR_Tests

#endif //STALKER_STLKR_TESTBASE_H
