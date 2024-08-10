//
// Created by hal9000 on 7/17/24.
//

#ifndef STALKER_SINGLETHREADOPERATIONSPERFORMANCETEST_H
#define STALKER_SINGLETHREADOPERATIONSPERFORMANCETEST_H
#include "../../STLKR_PerformanceTestBase.h"
#include "../../../../LinearAlgebra/ContiguousMemoryStorage/StalkerVector.h"

namespace STLKR_Tests{

    template<unsigned size>
    class SingleThreadOperationsPerformanceTest : public STLKR_PerformanceTestBase {
    public:
        SingleThreadOperationsPerformanceTest(const std::string &pathToTestClass) :
                STLKR_PerformanceTestBase("StalkerVector", pathToTestClass){}


        void runTest() override {
            _testCopy();
            _testFill();
            _testSum();
        }

    private:
        
        void _testCopy(){
            logs.addParameter("size", std::to_string(size));
            logs.addParameter("alignment", 64);
            logs.addParameter("operation", "copy");
            logs.addParameter("cores", 1);
            _copySIMD<float, 1>();
            _copySIMD<float, 4>();
            _copySIMD<float, 16>();
            _copySIMD<double, 1>();
            _copySIMD<double, 4>();
            _copySIMD<double, 16>();
            _copySIMD<int, 1>();
            _copySIMD<int, 4>();
            _copySIMD<int, 16>();
            _copySIMD<unsigned, 1>();
            _copySIMD<unsigned, 4>();
            _copySIMD<unsigned, 16>();
            _copySIMD<short, 1>();
            _copySIMD<short, 4>();
            _copySIMD<short, 16>();

            _copySTD<float>();
            _copySTD<double>();
            _copySTD<int>();
            _copySTD<unsigned>();
            _copySTD<short>();

            logs.exportToCSV(_path + "/CopyLogs", "copy");
            logs.clearAllLogs();
        }
        template<typename T, unsigned _unrollFactor>
        void _copySIMD(){
            StalkerVector<T, _unrollFactor> source(size, _manager);
            for (size_t i = 0; i < size; i++) {
                source[i] = static_cast<T>(i);
            }
            StalkerVector<T, _unrollFactor> v1(size, _manager);
            _manager.setAvailableCores(1);
            auto unroll = std::to_string(_unrollFactor);
            auto typeString = getTypeString<T>();
            logs.startSingleObservationTimer(typeString + "_SIMD_unroll_" + unroll , STLKR_TimeUnit::microseconds);
            v1.copy(source);
            logs.stopSingleObservationTimer(typeString + "_SIMD_unroll_" + unroll ,
                                            STLKR_TimeUnit::microseconds);
        }

        template<typename T>
        void _copySTD(){
            T* data1 = new T[size];
            T* data2 = new T[size];
            for (size_t i = 0; i < size; i++) {
                data1[i] = static_cast<T>(i);
            }
            logs.startSingleObservationTimer(getTypeString<T>() + "_memcpy", STLKR_TimeUnit::microseconds);
            std::memcpy(data1, data2, size * sizeof(T));
            logs.stopSingleObservationTimer(getTypeString<T>() + "_memcpy", STLKR_TimeUnit::microseconds);
            delete[] data1;
            delete[] data2;

            T* data3 = new T[size];
            T* data4 = new T[size];
            for (size_t i = 0; i < size; i++) {
                data4[i] = static_cast<T>(i);
            }
            logs.startSingleObservationTimer(getTypeString<T>() + "_loop", STLKR_TimeUnit::microseconds);
            for (size_t i = 0; i < size; i++) {
                data3[i] = data4[i];
            }
            logs.stopSingleObservationTimer(getTypeString<T>() + "_loop", STLKR_TimeUnit::microseconds);
            
            delete[] data3;
            delete[] data4;
        }

        void _testFill(){
            logs.addParameter("size", std::to_string(size));
            logs.addParameter("alignment", 64);
            logs.addParameter("operation", "fill");
            logs.addParameter("cores", 1);
            _fillSIMD<float, 1>();
            _fillSIMD<float, 4>();
            _fillSIMD<float, 16>();
            _fillSIMD<double, 1>();
            _fillSIMD<double, 4>();
            _fillSIMD<double, 16>();
            _fillSIMD<int, 1>();
            _fillSIMD<int, 4>();
            _fillSIMD<int, 16>();
            _fillSIMD<unsigned, 1>();
            _fillSIMD<unsigned, 4>();
            _fillSIMD<unsigned, 16>();
            _fillSIMD<short, 1>();
            _fillSIMD<short, 4>();
            _fillSIMD<short, 16>();

            _fillSTD<float>();
            _fillSTD<double>();
            _fillSTD<int>();
            _fillSTD<unsigned>();
            _fillSTD<short>();
    
            logs.exportToCSV(_path + "/FillLogs", "fill");
            logs.clearAllLogs();
        }

        template<typename T, unsigned _unrollFactor>
        void _fillSIMD(){
            StalkerVector<T, _unrollFactor> source(size, _manager);
            for (size_t i = 0; i < size; i++) {
                source[i] = static_cast<T>(i);
            }
            StalkerVector<T, _unrollFactor> v1(size, _manager);
            _manager.setAvailableCores(1);
            auto unroll = std::to_string(_unrollFactor);
            auto typeString = getTypeString<T>();
            logs.startSingleObservationTimer(typeString + "_SIMD_unroll_" + unroll , STLKR_TimeUnit::microseconds);
            v1.fill(static_cast<T>(666));
            logs.stopSingleObservationTimer(typeString + "_SIMD_unroll_" + unroll ,
                                            STLKR_TimeUnit::microseconds);
        }
    
        template<typename T>
        void _fillSTD(){
            T* data1 = new T[size];
            for (size_t i = 0; i < size; i++) {
                data1[i] = static_cast<T>(i);
            }
            logs.startSingleObservationTimer(getTypeString<T>() + "_fill", STLKR_TimeUnit::microseconds);
            std::fill(data1, data1 + size, static_cast<T>(666));
            logs.stopSingleObservationTimer(getTypeString<T>() + "_fill", STLKR_TimeUnit::microseconds);
            delete[] data1;
    
            T* data3 = new T[size];
            logs.startSingleObservationTimer(getTypeString<T>() + "_loop", STLKR_TimeUnit::microseconds);
            for (size_t i = 0; i < size; i++) {
                data3[i] = static_cast<T>(666);
            }
            logs.stopSingleObservationTimer(getTypeString<T>() + "_loop", STLKR_TimeUnit::microseconds);
            delete[] data3;
        }

        void _testSum(){
            logs.addParameter("size", std::to_string(size));
            logs.addParameter("alignment", 64);
            logs.addParameter("operation", "fill");
            logs.addParameter("cores", 1);
            _sumSIMD<float, 1>();
            _sumSIMD<float, 4>();
            _sumSIMD<float, 16>();
            _sumSIMD<double, 1>();
            _sumSIMD<double, 4>();
            _sumSIMD<double, 16>();
            _sumSIMD<int, 1>();
            _sumSIMD<int, 4>();
            _sumSIMD<int, 16>();
            _sumSIMD<unsigned, 1>();
            _sumSIMD<unsigned, 4>();
            _sumSIMD<unsigned, 16>();
            _sumSIMD<short, 1>();
            _sumSIMD<short, 4>();
            _sumSIMD<short, 16>();

            _sumSTD<float>();
            _sumSTD<double>();
            _sumSTD<int>();
            _sumSTD<unsigned>();
            _sumSTD<short>();

            logs.exportToCSV(_path + "/SumLogs", "sum");
            logs.clearAllLogs();
        }

        template<typename T, unsigned _unrollFactor>
        void _sumSIMD(){
            StalkerVector<T, _unrollFactor> v1(size, _manager);
            for (size_t i = 0; i < size; i++) {
                v1[i] = static_cast<T>(i);
            }
            _manager.setAvailableCores(1);
            auto typeString = getTypeString<T>();
            string unroll = std::to_string(_unrollFactor);
            logs.startSingleObservationTimer(typeString + "_SIMD_unroll_" + unroll , STLKR_TimeUnit::microseconds);
            auto sum = v1.sum();
            logs.stopSingleObservationTimer(typeString + "_SIMD_unroll_" + unroll , STLKR_TimeUnit::microseconds);
        }

        template<typename T>
        void _sumSTD(){
            T* data1 = new T[size];
            for (size_t i = 0; i < size; i++) {
                data1[i] = static_cast<T>(i);
            }
            logs.startSingleObservationTimer(getTypeString<T>() + "_accumulate", STLKR_TimeUnit::microseconds);
            std::accumulate(data1, data1 + size, static_cast<T>(0));
            logs.stopSingleObservationTimer(getTypeString<T>() + "_accumulate", STLKR_TimeUnit::microseconds);
            delete[] data1;

            T* data3 = new T[size];
            logs.startSingleObservationTimer(getTypeString<T>() + "_loop", STLKR_TimeUnit::microseconds);
            T sum = 0;
            for (size_t i = 0; i < size; i++) {
                sum += data3[i];
            }
            logs.stopSingleObservationTimer(getTypeString<T>() + "_loop", STLKR_TimeUnit::microseconds);
            delete[] data3;
        }

    };
}
#endif //STALKER_SINGLETHREADOPERATIONSPERFORMANCETEST_H
