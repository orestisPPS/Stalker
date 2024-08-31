//
// Created by hal9000 on 4/28/24.
//

#ifndef STALKER_STLKR_PERFORMANCETESTBASE_H
#define STALKER_STLKR_PERFORMANCETESTBASE_H

#include <filesystem>
#include "../../Utility/Logs.h"

#include "../../Utility/Logs.h"
#include "../STLKR_TestBase.h"

namespace STLKR_Tests {
    class STLKR_PerformanceTestBase : public STLKR_TestBase {
        public:
            explicit STLKR_PerformanceTestBase(const string& testName, const string& path, unsigned testRunCount = 1) :
                    STLKR_TestBase(testName), logs(Logs(testName)), _path(path), _testRunCount(testRunCount) {}

            Logs logs;
            
        protected:
            std::string _path;
            unsigned _testRunCount;
    };
}

#endif //STALKER_STLKR_PERFORMANCETESTBASE_H


