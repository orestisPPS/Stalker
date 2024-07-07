//
// Created by hal9000 on 4/29/24.
//

#include "STLKR_PerformanceTestBase.h"

namespace STLKR_Tests {
    STLKR_PerformanceTestBase::STLKR_PerformanceTestBase(const string& testName, const string& path) : STLKR_TestBase(testName),
                                                                                   logs(Logs(testName)),
                                                                                   _path(path){}
                                                                                   
} // STLKR_Tests