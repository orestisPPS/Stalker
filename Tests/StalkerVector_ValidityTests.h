//
// Created by hal9000 on 7/6/24.
//

#ifndef STALKER_STALKERPERFORMANCEVECTORTEST_H
#define STALKER_STALKERPERFORMANCEVECTORTEST_H
#include "STLKR_TestBase.h"
#include "../LinearAlgebra/ContiguousMemoryStorage/StalkerVector.h"

#endif //STALKER_STALKERPERFORMANCEVECTORTEST_H

namespace STLKR_Tests {
    
    template <unsigned unrollFactor>
    class StalkerVector_ValidityTests : public STLKR_TestBase {
    public:
        explicit StalkerVector_ValidityTests() : STLKR_TestBase("Stalker Performance Vector Test") {}

        void runTest() override {
            //_runGeneralVectorCases();
            _runSIMDVectorCases();
            //_manager.printInConsole();
        }

    protected:

        void _runGeneralVectorCases(){
            
            printTestCaseResult(_testConstructorWithSize<double>(), "Constructor with size (double)");
            printTestCaseResult(_testConstructorWithSize<float>(), "Constructor with size (float)");
            printTestCaseResult(_testConstructorWithSize<int>(), "Constructor with size (int)");
            printTestCaseResult(_testConstructorWithSize<short>(), "Constructor with size (short)");
            printTestCaseResult(_testConstructorWithSize<unsigned>(), "Constructor with size (unsigned)");
            
            printTestCaseResult(_testConstructorWithData<double>(), "Constructor with data (double)");
            printTestCaseResult(_testConstructorWithData<float>(), "Constructor with data (float)");
            printTestCaseResult(_testConstructorWithData<int>(), "Constructor with data (int)");
            printTestCaseResult(_testConstructorWithData<short>(), "Constructor with data (short)");
            printTestCaseResult(_testConstructorWithData<unsigned>(), "Constructor with data (unsigned)");
            
            printTestCaseResult(_testCopyConstructor(), "Copy constructor");
            printTestCaseResult(_testCopyAssignmentOperator(), "Copy assignment operator");
            printTestCaseResult(_testMoveConstructor(), "Move constructor");
            printTestCaseResult(_testMoveAssignmentOperator(), "Move assignment operator");
            printTestCaseResult(_testSize(), "Size");
            //Size in cache lines
            printTestCaseResult(_testSizeInCacheLines<float>(), "Size in cache lines (float)");
            printTestCaseResult(_testSizeInCacheLines<double>(), "Size in cache lines (double)");
            printTestCaseResult(_testSizeInCacheLines<int>(), "Size in cache lines (int)");
            printTestCaseResult(_testSizeInCacheLines<short>(), "Size in cache lines (short)");
            printTestCaseResult(_testSizeInCacheLines<unsigned>(), "Size in cache lines (unsigned)");
            
            printTestCaseResult(_testAlignment(), "Alignment");
            printTestCaseResult(_testElementAccessNonConst(), "Element access non-const");
            printTestCaseResult(_testElementAccessConst(), "Element access const");
            printTestCaseResult(_testBegin(), "Iterator begin");
            printTestCaseResult(_testEnd(), "Iterator end");
            printTestCaseResult(_testBeginConst(), "Iterator begin const");
            printTestCaseResult(_testEndConst(), "Iterator end const");
            printTestCaseResult(_testRange(), "Custom Range Iterator");
            
            //printTestCaseResult(_testSameCoreThreadAffinity(), "Set HyperThread affinity to same core");
        }

        template<typename T>
        bool _testConstructorWithSize() {
            auto size = 666;
            StalkerVector<T, unrollFactor> vec(size, 6, _manager);
            if (vec.size() != size) return false;
            for (unsigned i = 0; i < vec.size(); ++i) {
                if (vec[i] != 6) return false;
            }
            return true;
        }

        template<typename T>
        bool _testConstructorWithData() {
            //34 elements from 0 to 33
            T data[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
                        10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
                        20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
                        30, 31, 32, 33};
            StalkerVector<T, unrollFactor> vec(data, 5, _manager);
            if (vec.size() != 5) return false;
            for (unsigned i = 0; i < vec.size(); ++i) {
                if (vec[i] != data[i]) return false;
            }
            return true;
        }

        bool _testCopyConstructor() {
            StalkerVector<int, unrollFactor> vec1(10, 5, _manager);
            StalkerVector<int, unrollFactor> vec2(vec1);
            if (vec2.size() != vec1.size()) return false;
            for (unsigned i = 0; i < vec2.size(); ++i) {
                if (vec2[i] != vec1[i]) return false;
            }
            return true;
        }

        bool _testCopyAssignmentOperator() {
            StalkerVector<int, unrollFactor> vec1(10, 5, _manager);
            StalkerVector<int, unrollFactor> vec2(5, 3, _manager);
            vec2 = vec1;
            if (vec2.size() != vec1.size()) return false;
            for (unsigned i = 0; i < vec2.size(); ++i) {
                if (vec2[i] != vec1[i]) return false;
            }
            return true;
        }

        bool _testMoveConstructor() {
            StalkerVector<int, unrollFactor> vec1(10, 5, _manager);
            StalkerVector<int, unrollFactor> vec2(std::move(vec1));
            if (vec2.size() != 10) return false;
            for (unsigned i = 0; i < vec2.size(); ++i) {
                if (vec2[i] != 5) return false;
            }
            if (vec1.size() != 0) return false; // vec1 should be empty after move
            return true;
        }

        bool _testMoveAssignmentOperator() {
            StalkerVector<int, unrollFactor> vec1(10, 5, _manager);
            StalkerVector<int, unrollFactor> vec2(5, 3, _manager);
            vec2 = std::move(vec1);
            if (vec2.size() != 10) return false;
            for (unsigned i = 0; i < vec2.size(); ++i) {
                if (vec2[i] != 5) return false;
            }
            if (vec1.size() != 0) return false; // vec1 should be empty after move
            return true;
        }

        bool _testSize() {
            StalkerVector<int, unrollFactor> vec(10, 5, _manager);
            return vec.size() == 10;
        }
        
        template<typename T>
        bool _testSizeInCacheLines() {
            if (std::is_same<T, float>::value){
                StalkerVector<float, unrollFactor> vec(65, 5.0f, _manager); // 65 elements * 4 bytes each = 260 bytes, which is 5 cache lines of 64 bytes each
                return vec.sizeInCacheLines() == 5;
            }
            else if (std::is_same<T, double>::value){
                StalkerVector<double, unrollFactor> vec(33, 5, _manager); // 33 elements * 8 bytes each = 264 bytes, which is 5 cache lines of 64 bytes each
                return vec.sizeInCacheLines() == 5;
            }
            else if (std::is_same<T, int>::value) {
                StalkerVector<int, unrollFactor> vec(65, 5,_manager); // 65 elements * 4 bytes each = 260 bytes, which is 5 cache lines of 64 bytes each
                return vec.sizeInCacheLines() == 5;
            }
            else if (std::is_same<T, short>::value) {
                StalkerVector<short, unrollFactor> vec(129, 5, _manager); // 129 elements * 2 bytes each = 258 bytes, which is 5 cache lines of 64 bytes each
                return vec.sizeInCacheLines() == 5;
            }
            else if (std::is_same<T, unsigned>::value) {
                StalkerVector<unsigned, unrollFactor> vec(65, 5, _manager); // 65 elements * 4 bytes each = 260 bytes, which is 5 cache lines of 64 bytes each
                return vec.sizeInCacheLines() == 5;
            }
        }
        
        template<typename T>
        bool _testAVXRegisterSize(){
            if (std::is_same<T, float>::value){
                StalkerVector<float, unrollFactor> vec(32, 5.0f, _manager); // Assuming FLOAT_AVX_REGISTER_SIZE = 8
                return vec.getAVXRegisterSize() == FLOAT_AVX_REGISTER_SIZE;
            }
            else if (std::is_same<T, double>::value){
                StalkerVector<double, unrollFactor> vec(32, 5, _manager); // Assuming DOUBLE_AVX_REGISTER_SIZE = 4
                return vec.getAVXRegisterSize() == DOUBLE_AVX_REGISTER_SIZE;
            }
            else if (std::is_same<T, int>::value) {
                StalkerVector<int, unrollFactor> vec(32, 5, _manager); // Assuming INT_AVX_REGISTER_SIZE = 8
                return vec.getAVXRegisterSize() == INT_AVX_REGISTER_SIZE;
            }
            else if (std::is_same<T, short>::value) {
                StalkerVector<short, unrollFactor> vec(32, 5, _manager); // Assuming SHORT_AVX_REGISTER_SIZE = 16
                return vec.getAVXRegisterSize() == SHORT_AVX_REGISTER_SIZE;
            }
            else if (std::is_same<T, unsigned>::value) {
                StalkerVector<unsigned, unrollFactor> vec(32, 5, _manager); // Assuming UNSIGNED_AVX_REGISTER_SIZE = 8
                return vec.getAVXRegisterSize() == UNSIGNED_AVX_REGISTER_SIZE;
            }
        }

        bool _testAlignment() {
            StalkerVector<int, unrollFactor> vec(10, 5, _manager);
            return vec.alignment() == 64;
        }

        bool _testElementAccessNonConst() {
            StalkerVector<int, unrollFactor> vec(10, 5, _manager);
            vec[5] = 10;
            return vec[5] == 10;
        }

        bool _testElementAccessConst() {
            const StalkerVector<int, unrollFactor> vec(10, 5, _manager);
            return vec[5] == 5;
        }

        bool _testBegin() {
            StalkerVector<int, unrollFactor> vec(10, 5, _manager);
            return *(vec.begin()) == 5;
        }

        bool _testEnd() {
            StalkerVector<int, unrollFactor> vec(10, 5, _manager);
            return vec.end() == (vec.begin() + vec.size());
        }

        bool _testBeginConst() {
            const StalkerVector<int, unrollFactor> vec(10, 5, _manager);
            return *(vec.begin()) == 5;
        }

        bool _testEndConst() {
            const StalkerVector<int, unrollFactor> vec(10, 5, _manager);
            return vec.end() == (vec.begin() + vec.size());
        }

        bool _testRange() {
            StalkerVector<int, unrollFactor> vec(10, 5, _manager);
            auto range = vec.range(3, 7);

            // Check if the starting iterator points to the correct value
            bool startCorrect = *(range.first) == 5;

            // Check if the ending iterator points to the correct value
            bool endCorrect = (range.second == vec.begin() + 7);

            // Check if the distance between the start and end iterator is correct
            bool distanceCorrect = std::distance(range.first, range.second) == 4;

            return startCorrect && endCorrect && distanceCorrect;
        }


        void _runSIMDVectorCases(){
            size_t size = 666;

            //AVX register size
            
            //printTestCaseResult(_testAVXRegisterSize<float>(), "AVX register size (float)");
            //printTestCaseResult(_testAVXRegisterSize<double>(), "AVX register size (double)");
            //printTestCaseResult(_testAVXRegisterSize<int>(), "AVX register size (int)");
            //printTestCaseResult(_testAVXRegisterSize<short>(), "AVX register size (short)");
            //printTestCaseResult(_testAVXRegisterSize<unsigned>(), "AVX register size (unsigned)");
            //Deep copy SIMD
            //printTestCaseResult(_testDeepCopySIMD<double>(size), "Deep copy SIMD (double)");
            //printTestCaseResult(_testDeepCopySIMD<float>(size), "Deep copy SIMD (float)");
            //printTestCaseResult(_testDeepCopySIMD<int>(size), "Deep copy SIMD (int)");
            //printTestCaseResult(_testDeepCopySIMD<short>(size), "Deep copy SIMD (short)");
            //printTestCaseResult(_testDeepCopySIMD<unsigned>(size), "Deep copy SIMD (unsigned)");
            //Set value SIMD
            //printTestCaseResult(_testSetValueSIMD<double>(size), "Set value SIMD (double)");
            //printTestCaseResult(_testSetValueSIMD<float>(size), "Set value SIMD (float)");
            //printTestCaseResult(_testSetValueSIMD<int>(size), "Set value SIMD (int)");
            //printTestCaseResult(_testSetValueSIMD<short>(size), "Set value SIMD (short)");
            //printTestCaseResult(_testSetValueSIMD<unsigned>(size), "Set value SIMD (unsigned)");
            //Are equal
            //printTestCaseResult(_testAreEqual<double>(size), "Are equal (double)");
            //printTestCaseResult(_testAreEqual<float>(size), "Are equal (float)");
            //printTestCaseResult(_testAreEqual<int>(size), "Are equal (int)");
            //printTestCaseResult(_testAreEqual<short>(size), "Are equal (short)");
            //printTestCaseResult(_testAreEqual<unsigned>(size), "Are equal (unsigned)");
            //Are not equal
            //printTestCaseResult(areNotEqual<double>(size), "Are not equal (double)");
            //printTestCaseResult(areNotEqual<float>(size), "Are not equal (float)");
            //printTestCaseResult(areNotEqual<int>(size), "Are not equal (int)");
            //printTestCaseResult(areNotEqual<short>(size), "Are not equal (short)");
            //printTestCaseResult(areNotEqual<unsigned>(size), "Are not equal (unsigned)");
            
            //add
            //printTestCaseResult(_testAdd<float>(size), "add (float)");
            //printTestCaseResult(_testAdd<double>(size), "add (double)");
            //printTestCaseResult(_testAdd<int>(size), "add (int)");
            //printTestCaseResult(_testAdd<short>(size), "add (short)");
            //printTestCaseResult(_testAdd<unsigned>(size), "add (unsigned)");
            
            //subtract
            printTestCaseResult(_testSubtract<float>(size), "subtract (float)");
            printTestCaseResult(_testSubtract<double>(size), "subtract (double)");
            printTestCaseResult(_testSubtract<int>(size), "subtract (int)");
            printTestCaseResult(_testSubtract<short>(size), "subtract (short)");
            printTestCaseResult(_testSubtract<unsigned>(size), "subtract (unsigned)");

            //multiply
            printTestCaseResult(_testMultiply<float>(size), "multiply (float)");
            printTestCaseResult(_testMultiply<double>(size), "multiply (double)");
            printTestCaseResult(_testMultiply<int>(size), "multiply (int)");
            printTestCaseResult(_testMultiply<short>(size), "multiply (short)");
            printTestCaseResult(_testMultiply<unsigned>(size), "multiply (unsigned)");
        }

        template<typename T>
        bool _testDeepCopySIMD(size_t size) {
            StalkerVector<T, unrollFactor> vec1(size, 0, _manager);
            StalkerVector<T, unrollFactor> vec2(size, 0, _manager);
            for (unsigned i = 0; i < vec1.size(); ++i) {
                vec1[i] = i;
            }
            vec2.copy(vec1);
            for (unsigned i = 0; i < vec1.size(); ++i) {
                if (vec1[i] != vec2[i]) return false;
            }
            return true;
        }

        template<typename T>
        bool _testSetValueSIMD(size_t size) {
            StalkerVector<T, unrollFactor> vec(size, 0, _manager);
            vec.setValue(5);
            for (unsigned i = 0; i < vec.size(); ++i) {
                if (vec[i] != 5) return false;
            }
            return true;
        }

        template<typename T>
        bool _testAreEqual(size_t size) {
            StalkerVector<T, unrollFactor> vec1(size, 0, _manager);
            StalkerVector<T, unrollFactor> vec2(size, 0, _manager);
            for (unsigned i = 0; i < vec1.size(); ++i) {
                vec1[i] = i;
                vec2[i] = i;
            }
            return vec1.areEqual(vec2);
        }

        template<typename T>
        bool areNotEqual(size_t size){
            StalkerVector<T, unrollFactor> vec1(size, 0, _manager);
            StalkerVector<T, unrollFactor> vec2(size, 0, _manager);
            for (unsigned i = 0; i < vec1.size(); ++i) {
                vec1[i] = i;
                vec2[i] = vec1.size() - 1 - i;
            }
            return !vec1.areEqual(vec2);
        }

        template<typename T>
        bool _testAdd(size_t size) {
            StalkerVector<T, unrollFactor> vec1(size, 0, _manager);
            StalkerVector<T, unrollFactor> vec2(size, 0, _manager);
            StalkerVector<T, unrollFactor> result(size, 0, _manager);
            
            for (unsigned i = 0; i < vec1.size(); ++i) {
                vec1[i] = 1;
                vec2[i] = 1;
            }

            if (std::is_same<T, float>::value || std::is_same<T, double>::value){
                vec1.add(vec2, result, 0.5, 1.5);
                for (unsigned i = 0; i < vec1.size(); ++i)
                    if (result[i] != 2) return false;
                
            }
            if (std::is_same<T, int>::value || std::is_same<T, short>::value || std::is_same<T, unsigned>::value){
                vec1.add(vec2, result, 0, 3);
                for (unsigned i = 0; i < vec1.size(); ++i)
                    if (result[i] != 3) return false;
                
            }
            return true;
        }

        template<typename T>
        bool _testSubtract(size_t size) {
            StalkerVector<T, unrollFactor> vec1(size, 0, _manager);
            StalkerVector<T, unrollFactor> vec2(size, 0, _manager);
            StalkerVector<T, unrollFactor> result(size, 0, _manager);

            for (unsigned i = 0; i < vec1.size(); ++i) {
                vec1[i] = 1;
                vec2[i] = 1;
            }

            if (std::is_same<T, float>::value || std::is_same<T, double>::value){
                vec1.subtract(vec2, result, 0.5, -1.5);
                for (unsigned i = 0; i < vec1.size(); ++i)
                    if (result[i] != 2) return false;

            }
            if (std::is_same<T, int>::value || std::is_same<T, short>::value || std::is_same<T, unsigned>::value){
                vec1.subtract(vec2, result, 0, -3);
                for (unsigned i = 0; i < vec1.size(); ++i)
                    if (result[i] != 3) return false;

            }
            return true;
        }
        
        template<typename T>
        bool _testMultiply(size_t size) {
            StalkerVector<T, unrollFactor> vec1(size, 0, _manager);
            StalkerVector<T, unrollFactor> vec2(size, 0, _manager);
            StalkerVector<T, unrollFactor> result(size, 0, _manager);

            for (unsigned i = 0; i < vec1.size(); ++i) {
                vec1[i] = 1;
                vec2[i] = 1;
            }

            if (std::is_same<T, float>::value || std::is_same<T, double>::value){
                vec1.multiply(vec2, result, 0.5, 1.5);
                for (unsigned i = 0; i < vec1.size(); ++i)
                    if (result[i] != 0.75) return false;

            }
            if (std::is_same<T, int>::value || std::is_same<T, short>::value || std::is_same<T, unsigned>::value){
                vec1.multiply(vec2, result, 333, 2);
                for (unsigned i = 0; i < vec1.size(); ++i)
                    if (result[i] != 666) return false;

            }
            return true;
        }

    };

} // STLKR_Tests