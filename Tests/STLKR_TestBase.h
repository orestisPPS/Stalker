//
// Created by hal9000 on 4/29/24.
//

#ifndef STALKER_STLKR_TESTBASE_H
#define STALKER_STLKR_TESTBASE_H

#include <utility>
#include <string>
#include <iostream>
#include <limits.h>
#include "../Threading/CPUTopology/CPU_Manager.h"


namespace STLKR_Tests {

    class STLKR_TestBase{
            virtual void runTest() = 0;
            
            void runTest(unsigned int iterations);

        protected:

            CPU_Manager _manager;

            explicit STLKR_TestBase(std::string testName);
            
            std::string _testName;
            
            //void _welcomeMessage();
            
            static void printTestCaseResult(bool success, const std::string& name);
            
            static void printSuccess();

            static void printFailure();
            
            template<typename T>
            static std::string getTypeString(){
                if (std::is_same<T, float>::value) {
                    return "float";
                } else if (std::is_same<T, double>::value) {
                    return "double";
                } else if (std::is_same<T, int>::value) {
                    return "int";
                } else if (std::is_same<T, short>::value) {
                    return "short";
                } else if (std::is_same<T, unsigned>::value) {
                    return "unsigned";
                } else {
                    return "unknown";
                }
            }
    };

} // STLKR_Tests

#endif //STALKER_STLKR_TESTBASE_H
