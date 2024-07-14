//
// Created by hal9000 on 7/11/24.
//

#ifndef STALKER_STALKERVECTOR_PERFORMANCETESTS_H
#define STALKER_STALKERVECTOR_PERFORMANCETESTS_H
#include "STLKR_PerformanceTestBase.h"
#include "../../LinearAlgebra/ContiguousMemoryStorage/StalkerVector.h"

namespace STLKR_Tests{
    
    template<typename T,unsigned size, unsigned unrollFactor>
    class StalkerVector_PerformanceTests : public STLKR_PerformanceTestBase {
    public:
        StalkerVector_PerformanceTests(const std::string &path) :
            STLKR_PerformanceTestBase("StalkerVector", path){}
        
        
        void runTest() override {
            logs.addParameter("size", std::to_string(size));
            logs.addParameter("alignment", 64);
            logs.addParameter("type", getTypeString<T>());
            logs.addParameter("operation", "copy");
            logs.addParameter("unrollFactor", unrollFactor);
            _runDeepCopyTest();
            logs.exportToCSV(_path, "StalkerVectorTest");
            logs.clearAllLogs();
        }
        
    private:
        void _runDeepCopyTest(){
            StalkerVector<T, unrollFactor> source(size, _manager);
            for (size_t i = 0; i < size; i++) {
                source[i] = static_cast<T>(i);
            }
            StalkerVector<T, unrollFactor> v1(size, _manager);
            source.setAvailableCores(1);
            v1.setAvailableCores(1);
            logs.startSingleObservationTimer("SIMD_cores1", STLKR_TimeUnit::microseconds);
            v1.copy(source);
            logs.stopSingleObservationTimer("SIMD_cores1");

            StalkerVector<T, unrollFactor> v2(size, _manager);
            source.setAvailableCores(2);
            v2.setAvailableCores(2);
            logs.startSingleObservationTimer("SIMD_cores2", STLKR_TimeUnit::microseconds);
            v2.copy(source);
            logs.stopSingleObservationTimer("SIMD_cores2");

            StalkerVector<T, unrollFactor> v3(size, _manager);
            source.setAvailableCores(4);
            v3.setAvailableCores(4);
            logs.startSingleObservationTimer("SIMD_cores4", STLKR_TimeUnit::microseconds);
            v3.copy(source);
            logs.stopSingleObservationTimer("SIMD_cores4");
            
            
//            auto copyJob = [&](size_t start, size_t end){
//                for (size_t i = start; i < end; i++) {
//                    data4[i] = data2[i];
//                }
//            };

            T* data1 = new T[size];
            T* data2 = new T[size];
            for (size_t i = 0; i < size; i++) {
                data1[i] = static_cast<T>(i);
            }
            logs.startSingleObservationTimer("memcpy_noSIMD", STLKR_TimeUnit::microseconds);
            std::memcpy(data1, data2, size * sizeof(T));
            logs.stopSingleObservationTimer("memcpy_noSIMD");
            delete[] data1;
            delete[] data2;
            
            T* data3 = new T[size];
            T* data4 = new T[size];
            logs.startSingleObservationTimer("loopCopy_noSIMD", STLKR_TimeUnit::microseconds);
            for (size_t i = 0; i < size; i++) {
                data3[i] = data4[i];
            }
            logs.stopSingleObservationTimer("loopCopy_noSIMD");
            
            delete[] data3;
            delete[] data4;
        }
    };
}
#endif //STALKER_STALKERVECTOR_PERFORMANCETESTS_H
