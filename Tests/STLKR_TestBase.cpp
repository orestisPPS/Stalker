//
// Created by hal9000 on 4/29/24.
//

#include "STLKR_TestBase.h"

namespace STLKR_Tests {
    
    STLKR_TestBase::STLKR_TestBase(std::string testName) : _testName(std::move(testName)) {
        //_welcomeMessage();
    }

    void STLKR_TestBase::runTest(unsigned int iterations) {
        for (unsigned int i = 0; i < iterations; i++) {
            runTest();
        }
    }

    void STLKR_TestBase::printSuccess() {
        std::cout << "\033[1;32m[PASSED ]\033[0m\n";  // This adds a green [PASSED] indicator
    }

    void STLKR_TestBase::printFailure() {
        std::cout << "\033[1;31m[FAILED ]\033[0m\n";  // This adds a red [FAILED] indicator
    }

    std::string STLKR_TestBase::boolToOnOff(bool value, const std::string& name) {
        return value ? name + "_on" : name + "_off";
    }

//    void STLKR_TestBase::_welcomeMessage() {
//        //std::cout << "========" << _testName << "========" << std::endl;
//    }


} // STLKR_Tests