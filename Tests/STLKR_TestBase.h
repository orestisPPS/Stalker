//
// Created by hal9000 on 4/29/24.
//

#ifndef STALKER_STLKR_TESTBASE_H
#define STALKER_STLKR_TESTBASE_H

#include <utility>
#include <string>
#include <iostream>


namespace STLKR_Tests {

    class STLKR_TestBase{
        
            virtual void runTest() = 0;
            
            void runTest(unsigned int iterations);

        protected:
        
            explicit STLKR_TestBase(std::string testName);
            
            std::string _testName;
            
            void _welcomeMessage();
            
            virtual void logTestCaseStart(const std::string& name) = 0;

            static void printSuccess();

            static void printFailure();

    };

} // STLKR_Tests

#endif //STALKER_STLKR_TESTBASE_H
