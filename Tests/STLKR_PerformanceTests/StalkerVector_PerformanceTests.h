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
            _runDeepCopyTest();
            logs.exportToCSV(_path, "StalkerVectorTest");
            logs.clearAllLogs();
        }
        
    private:
        void _runDeepCopyTest(){
            StalkerVector<T, unrollFactor> vector1(size, _manager);
            StalkerVector<T, unrollFactor> vector2(size, _manager);
            vector1.setAvailableCores(1);
            vector2.setAvailableCores(1);
            logs.startSingleObservationTimer("deep_copy_SIMD_noPrefetch_cores1", STLKR_TimeUnit::microseconds);
            vector1.deepCopy(vector2);
            logs.stopSingleObservationTimer("deep_copy_SIMD_noPrefetch_cores1");

            vector1.setAvailableCores(2);
            vector2.setAvailableCores(2);
            logs.startSingleObservationTimer("deep_copy_SIMD_noPrefetch_cores2", STLKR_TimeUnit::microseconds);
            vector1.deepCopy(vector2);
            logs.stopSingleObservationTimer("deep_copy_SIMD_noPrefetch_cores2");
            
            
            T* data1 = new T[size];
            T* data2 = new T[size];
            T* data3 = new T[size];
            T* data4 = new T[size];
            
            auto copyJob = [&](size_t start, size_t end){
                for (size_t i = start; i < end; i++) {
                    data4[i] = data2[i];
                }
            };
            for (size_t i = 0; i < size; i++) {
                data1[i] = static_cast<T>(i);
                data3[i] = static_cast<T>(i);
            }
            
            logs.startSingleObservationTimer("memcpy_noSIMD_noPrefetch", STLKR_TimeUnit::microseconds);
            std::memcpy(data1, data2, size * sizeof(T));
            logs.stopSingleObservationTimer("memcpy_noSIMD_noPrefetch");
            
            logs.startSingleObservationTimer("loopCopy_noSIMD_noPrefetch", STLKR_TimeUnit::microseconds);
            for (size_t i = 0; i < size; i++) {
                data3[i] = data4[i];
            }
            logs.stopSingleObservationTimer("loopCopy_noSIMD_noPrefetch");

            delete[] data1;
            delete[] data2;
            delete[] data3;
            delete[] data4;
        }
    };
}
#endif //STALKER_STALKERVECTOR_PERFORMANCETESTS_H
