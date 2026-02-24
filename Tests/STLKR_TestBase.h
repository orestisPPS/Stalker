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

#pragma once

#include <utility>
#include <string>
#include <iostream>
#include <memory>
#include <vector>
#include <array>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <filesystem>
#include <limits.h>
#include <cstdint>
#include <cassert>
#include <cstring>
#include <type_traits>
#include "TestUtility.h"
#include <Stalker/Mathematics/Linspace.h>
#include <Stalker/Memory/MemoryOperations.h>
#include <Stalker/Memory/Allocators.h>
#include <Stalker/Mathematics/Random.h>
#include <Stalker/Utility/Logs.h>
#include <Stalker/Core/Units.h>

namespace STLKR_Tests {
    using namespace Stalker::Core;
    using namespace Stalker::Mathematics;
    using namespace Stalker::Memory;
    using namespace Stalker::Utility;

    class STLKR_TestBase {

    public:

        explicit STLKR_TestBase(std::string testName) : _logs(Logs(testName)), _testName(testName) 
        {
            auto fullPath = std::filesystem::absolute(std::filesystem::path(__FILE__));
            auto basePath = fullPath.parent_path().parent_path();
            setPath(testName, basePath.string());
            TestUtility::printTestTitle(_testName, "=", T_Color::BRIGHT_WHITE);
        }


        void runTest(unsigned int iterations) {
            for (unsigned int i = 0; i < iterations; i++) {
                runTest();
            }
        }

        virtual void runTest() = 0;


        const std::string& getPath(const std::string& pathName) {
            return _paths[pathName];
        }

        void setPath(const std::string& pathName, const std::string& path) {
            auto createDirectory = TestUtility::mkdir(path);
            if (createDirectory)
                _paths[pathName] = path;
        }

    protected:
        Logs _logs;
        unsigned _caseCounter = 0;
        std::string _testName;
        std::unordered_map<std::string, std::string> _paths;

        static void printSuccess() {
            std::cout << "\033[1;32m[PASSED]\033[0m";
        }

        static void printFailure() {
            std::cout << "\033[1;31m[FAILED]\033[0m";
        }


        void resetCaseCounting() {
            _caseCounter = 0;
        }


        void printTestCaseResult(bool success, const std::string& name) {
            std::cout << "Test " << _caseCounter++ << ": ";
            if (success)
                printSuccess();
            else
                printFailure();
            std::cout << " " << name << std::endl;
        }

        static void printTestCaseResult(bool success, const std::string& testNumber, const std::string& testName) {
            std::cout << testNumber << " ";
            if (success)
                printSuccess();
            else
                printFailure();
            std::cout << " " << testName << std::endl;
        }

    };

} // namespace STLKR_Tests
