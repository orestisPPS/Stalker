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

#ifndef STALKER_TESTS_STALKER_TEST_RUNNER_H
#define STALKER_TESTS_STALKER_TEST_RUNNER_H

// #include "StalkerMatrix/IteratorTests.h"
// #include "StalkerMatrix/FixedStridePtrBufferTests.h"
// #include "StalkerMatrix/DataAccessTests.h"
#include "Mathematics/LinspaceTests.h"
#include "Mathematics/MetaMathTests.h"
#include "Mathematics/IntegrationTests.h"
#include "Mathematics/DifferentiationTests.h"
#include "Memory/AllocatorsTests.h"
#include "Memory/MemoryOperationsTests.h"
#include "Mathematics/MathOperationsTests.h"
#include "Mathematics/MatrixMathTests.h"

namespace STLKR_Tests {
    class StalkerTestRunner {
    public:
        StalkerTestRunner() = default;
        ~StalkerTestRunner() = default;

        void runTests() {
            std::cout << "SLET" << std::endl;

            LinspaceTests().runTest();
            MetaMathTests<double>().runTest();
            IntegrationTests().runTest();
            DifferentiationTests().runTest();
            TypeTraitsSIMDTests().runTest();
            AllocatorsTests().runTest();
            MemoryOperationsTests(2'000).runTest();
            MathOperationsTests(2'000).runTest();
            MatrixMathTests(333003).runTest();

            // IteratorTests().runTest();
            // FixedStridePtrBufferTests().runTest(); //NOT HOMOGENEOUS
            // DataAccessTests<double>().runTest();
            std::cout << "SLET" << std::endl;
        }
    };
}

#endif // STALKER_TESTS_STALKER_TEST_RUNNER_H