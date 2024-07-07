//
// Created by hal9000 on 7/6/24.
//

#ifndef STALKER_STALKERPERFORMANCEVECTORTEST_H
#define STALKER_STALKERPERFORMANCEVECTORTEST_H
#include "STLKR_TestBase.h"
#include "../LinearAlgebra/ContiguousMemoryStorage/StalkerPerformanceVector.h"

#endif //STALKER_STALKERPERFORMANCEVECTORTEST_H

namespace STLKR_Tests {
    template <unsigned unrollFactor>
    class StalkerPerformanceVectorTest : public STLKR_TestBase {
    public:
        explicit StalkerPerformanceVectorTest() : STLKR_TestBase("Stalker Performance Vector Test") {}

        void runTest() override {
            printTestCaseResult(_testConstructorWithSize(), "Constructor with size");
            printTestCaseResult(_testConstructorWithData(), "Constructor with data");
            printTestCaseResult(_testCopyConstructor(), "Copy constructor");
            printTestCaseResult(_testCopyAssignmentOperator(), "Copy assignment operator");
            printTestCaseResult(_testMoveConstructor(), "Move constructor");
            printTestCaseResult(_testMoveAssignmentOperator(), "Move assignment operator");
            printTestCaseResult(_testSize(), "Size");
            printTestCaseResult(_testSizeInCacheLinesDouble(), "Size in cache lines (double)");
            printTestCaseResult(_testSizeInAVXRegistersDouble(), "Size in AVX registers (double)");
            printTestCaseResult(_testAVX2RegisterSizeDouble(), "AVX2 register size (double)");
            printTestCaseResult(_testSizeInCacheLinesFloat(), "Size in cache lines (float)");
            printTestCaseResult(_testSizeInAVXRegistersFloat(), "Size in AVX registers (float)");
            printTestCaseResult(_testAVX2RegisterSizeFloat(), "AVX2 register size (float)");
            printTestCaseResult(_testSizeInCacheLinesInt(), "Size in cache lines (int)");
            printTestCaseResult(_testSizeInAVXRegistersInt(), "Size in AVX registers (int)");
            printTestCaseResult(_testAVX2RegisterSizeInt(), "AVX2 register size (int)");
            printTestCaseResult(_testSizeInCacheLinesShort(), "Size in cache lines (short)");
            printTestCaseResult(_testSizeInAVXRegistersShort(), "Size in AVX registers (short)");
            printTestCaseResult(_testAVX2RegisterSizeShort(), "AVX2 register size (short)");
            printTestCaseResult(_testSizeInCacheLinesUnsigned(), "Size in cache lines (unsigned)");
            printTestCaseResult(_testSizeInAVXRegistersUnsigned(), "Size in AVX registers (unsigned)");
            printTestCaseResult(_testAVX2RegisterSizeUnsigned(), "AVX2 register size (unsigned)");
            printTestCaseResult(_testAlignment(), "Alignment");
            printTestCaseResult(_testElementAccessNonConst(), "Element access non-const");
            printTestCaseResult(_testElementAccessConst(), "Element access const");
            printTestCaseResult(_testBegin(), "Iterator begin");
            printTestCaseResult(_testEnd(), "Iterator end");
            printTestCaseResult(_testBeginConst(), "Iterator begin const");
            printTestCaseResult(_testEndConst(), "Iterator end const");
            printTestCaseResult(_testRange(), "Custom Range Iterator");
        }

    protected:

        static bool _testConstructorWithSize() {
            StalkerPerformanceVector<int, unrollFactor> vec(10, 5);
            if (vec.size() != 10) return false;
            for (unsigned i = 0; i < vec.size(); ++i) {
                if (vec[i] != 5) return false;
            }
            return true;
        }

        static bool _testConstructorWithData() {
            int data[] = {1, 2, 3, 4, 5};
            StalkerPerformanceVector<int, unrollFactor> vec(data, 5);
            if (vec.size() != 5) return false;
            for (unsigned i = 0; i < vec.size(); ++i) {
                if (vec[i] != data[i]) return false;
            }
            return true;
        }

        static bool _testCopyConstructor() {
            StalkerPerformanceVector<int, unrollFactor> vec1(10, 5);
            StalkerPerformanceVector<int, unrollFactor> vec2(vec1);
            if (vec2.size() != vec1.size()) return false;
            for (unsigned i = 0; i < vec2.size(); ++i) {
                if (vec2[i] != vec1[i]) return false;
            }
            return true;
        }

        static bool _testCopyAssignmentOperator() {
            StalkerPerformanceVector<int, unrollFactor> vec1(10, 5);
            StalkerPerformanceVector<int, unrollFactor> vec2(5, 3);
            vec2 = vec1;
            if (vec2.size() != vec1.size()) return false;
            for (unsigned i = 0; i < vec2.size(); ++i) {
                if (vec2[i] != vec1[i]) return false;
            }
            return true;
        }

        static bool _testMoveConstructor() {
            StalkerPerformanceVector<int, unrollFactor> vec1(10, 5);
            StalkerPerformanceVector<int, unrollFactor> vec2(std::move(vec1));
            if (vec2.size() != 10) return false;
            for (unsigned i = 0; i < vec2.size(); ++i) {
                if (vec2[i] != 5) return false;
            }
            if (vec1.size() != 0) return false; // vec1 should be empty after move
            return true;
        }

        static bool _testMoveAssignmentOperator() {
            StalkerPerformanceVector<int, unrollFactor> vec1(10, 5);
            StalkerPerformanceVector<int, unrollFactor> vec2(5, 3);
            vec2 = std::move(vec1);
            if (vec2.size() != 10) return false;
            for (unsigned i = 0; i < vec2.size(); ++i) {
                if (vec2[i] != 5) return false;
            }
            if (vec1.size() != 0) return false; // vec1 should be empty after move
            return true;
        }

        static bool _testSize() {
            StalkerPerformanceVector<int, unrollFactor> vec(10, 5);
            return vec.size() == 10;
        }

        static bool _testSizeInCacheLinesDouble() {
            StalkerPerformanceVector<double, unrollFactor> vec(65, 5.0); // 65 elements * 8 bytes each = 520 bytes, which is 9 cache lines of 64 bytes each
            return vec.sizeInCacheLines() == 9;
        }

        static bool _testSizeInAVXRegistersDouble() {
            StalkerPerformanceVector<double, unrollFactor> vec(33, 5.0); // Assuming DOUBLE_AVX_REGISTER_SIZE = 4, 33 elements / 4 per register = 9 AVX registers
            return vec.sizeInAVXRegisters() == 9;
        }

        static bool _testAVX2RegisterSizeDouble() {
            StalkerPerformanceVector<double, unrollFactor> vec(32, 5.0); // Assuming DOUBLE_AVX_REGISTER_SIZE = 4
            return vec.AVX2RegisterSize() == DOUBLE_AVX_REGISTER_SIZE;
        }

        static bool _testSizeInCacheLinesFloat() {
            StalkerPerformanceVector<float, unrollFactor> vec(65, 5.0f); // 65 elements * 4 bytes each = 260 bytes, which is 5 cache lines of 64 bytes each
            return vec.sizeInCacheLines() == 5;
        }

        static bool _testSizeInAVXRegistersFloat() {
            StalkerPerformanceVector<float, unrollFactor> vec(33, 5.0f); // Assuming FLOAT_AVX_REGISTER_SIZE = 8, 33 elements / 8 per register = 5 AVX registers
            return vec.sizeInAVXRegisters() == 5;
        }

        static bool _testAVX2RegisterSizeFloat() {
            StalkerPerformanceVector<float, unrollFactor> vec(32, 5.0f); // Assuming FLOAT_AVX_REGISTER_SIZE = 8
            return vec.AVX2RegisterSize() == FLOAT_AVX_REGISTER_SIZE;
        }

        static bool _testSizeInCacheLinesInt() {
            StalkerPerformanceVector<int, unrollFactor> vec(65, 5); // 65 elements * 4 bytes each = 260 bytes, which is 5 cache lines of 64 bytes each
            return vec.sizeInCacheLines() == 5;
        }

        static bool _testSizeInAVXRegistersInt() {
            StalkerPerformanceVector<int, unrollFactor> vec(33, 5); // Assuming INT_AVX_REGISTER_SIZE = 8, 33 elements / 8 per register = 5 AVX registers
            return vec.sizeInAVXRegisters() == 5;
        }

        static bool _testAVX2RegisterSizeInt() {
            StalkerPerformanceVector<int, unrollFactor> vec(32, 5); // Assuming INT_AVX_REGISTER_SIZE = 8
            return vec.AVX2RegisterSize() == INT_AVX_REGISTER_SIZE;
        }

        static bool _testSizeInCacheLinesShort() {
            StalkerPerformanceVector<short, unrollFactor> vec(129, 5); // 129 elements * 2 bytes each = 258 bytes, which is 5 cache lines of 64 bytes each
            return vec.sizeInCacheLines() == 5;
        }

        static bool _testSizeInAVXRegistersShort() {
            StalkerPerformanceVector<short, unrollFactor> vec(33, 5); // Assuming SHORT_AVX_REGISTER_SIZE = 16, 33 elements / 16 per register = 3 AVX registers
            return vec.sizeInAVXRegisters() == 3;
        }

        static bool _testAVX2RegisterSizeShort() {
            StalkerPerformanceVector<short, unrollFactor> vec(32, 5); // Assuming SHORT_AVX_REGISTER_SIZE = 16
            return vec.AVX2RegisterSize() == SHORT_AVX_REGISTER_SIZE;
        }

        static bool _testSizeInCacheLinesUnsigned() {
            StalkerPerformanceVector<unsigned, unrollFactor> vec(65, 5u); // 65 elements * 4 bytes each = 260 bytes, which is 5 cache lines of 64 bytes each
            return vec.sizeInCacheLines() == 5;
        }

        static bool _testSizeInAVXRegistersUnsigned() {
            StalkerPerformanceVector<unsigned, unrollFactor> vec(33, 5u); // Assuming UNSIGNED_AVX_REGISTER_SIZE = 8, 33 elements / 8 per register = 5 AVX registers
            return vec.sizeInAVXRegisters() == 5;
        }

        static bool _testAVX2RegisterSizeUnsigned() {
            StalkerPerformanceVector<unsigned, unrollFactor> vec(32, 5u); // Assuming UNSIGNED_AVX_REGISTER_SIZE = 8
            return vec.AVX2RegisterSize() == UNSIGNED_AVX_REGISTER_SIZE;
        }

        static bool _testAlignment() {
            StalkerPerformanceVector<int, unrollFactor> vec(10, 5);
            return vec.alignment() == 64;
        }

        static bool _testElementAccessNonConst() {
            StalkerPerformanceVector<int, unrollFactor> vec(10, 5);
            vec[5] = 10;
            return vec[5] == 10;
        }

        static bool _testElementAccessConst() {
            const StalkerPerformanceVector<int, unrollFactor> vec(10, 5);
            return vec[5] == 5;
        }

        static bool _testBegin() {
            StalkerPerformanceVector<int, unrollFactor> vec(10, 5);
            return *(vec.begin()) == 5;
        }

        static bool _testEnd() {
            StalkerPerformanceVector<int, unrollFactor> vec(10, 5);
            return vec.end() == (vec.begin() + vec.size());
        }

        static bool _testBeginConst() {
            const StalkerPerformanceVector<int, unrollFactor> vec(10, 5);
            return *(vec.begin()) == 5;
        }

        static bool _testEndConst() {
            const StalkerPerformanceVector<int, unrollFactor> vec(10, 5);
            return vec.end() == (vec.begin() + vec.size());
        }

        static bool _testRange() {
            StalkerPerformanceVector<int, unrollFactor> vec(10, 5);
            auto range = vec.range(3, 7);

            // Check if the starting iterator points to the correct value
            bool startCorrect = *(range.first) == 5;

            // Check if the ending iterator points to the correct value
            bool endCorrect = (range.second == vec.begin() + 7);

            // Check if the distance between the start and end iterator is correct
            bool distanceCorrect = std::distance(range.first, range.second) == 4;

            return startCorrect && endCorrect && distanceCorrect;
        }

    };

} // STLKR_Tests