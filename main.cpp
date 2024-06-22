#include <iostream>
#include "Tests/STLKR_PerformanceTests/AVX_PerformanceTest.h"
#include "LinearAlgebra/ContiguousMemoryStorage/NumericalVector.h"
#include "STLKR_HighPerformance/STLKR_Threading/STLKR_Thread_OperationsLinux.h"
#include "STLKR_HighPerformance/STLKR_MachineSetup/STLKR_Machine_CPUTopologyLinux.h"
#include <chrono>

int main() {
    auto size = 400000000;
    auto data1 = new double[size];
    auto data2 = new double[size];
    auto result = new double[size];
    
    for (int i = 0; i < size; i++) {
        data1[i] = i;
        data2[i] = i;
    }
    auto additionJob = [&](unsigned startIndex, unsigned endIndex) {
        for (unsigned i = startIndex; i < endIndex; i++) {
            result[i] = data1[i] + data2[i];
            //std::cout << "data1[" << i << "] = " << data1[i] << ", data2[" << i << "] = " << data2[i] << ", result[" << i << "] = " << result[i] << std::endl;
        }
    };

    auto cpuTopology = STLKR_Machine_CPUTopologyLinux();
    auto cores = cpuTopology.getPhysicalCores();
    auto singleCore = std::vector<STLKR_Machine_Core*>{cores[0], cores[1]};
    auto start = std::chrono::high_resolution_clock::now();
    STLKR_Thread_OperationsLinux::executeJob(additionJob, size, singleCore, true);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "MultiThread Total time taken: " << duration << " ms" << std::endl;
    delete[] result;

    auto result2 = new double[size];
    
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < size; i++) {
        result2[i] = data1[i] + data2[i];
    }
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "SingleThread Total time taken: " << duration << " ms" << std::endl;
    
    cout << "Test completed successfully" << endl;
    delete[] data1;
    delete[] data2;
    delete[] result2;
    
    auto exportPath = "../Tests/STLKR_PerformanceTests/logs";
    //No memory bugs with 40000000 elements
    cout << "ΔΩΣΕ ΓΚΑΖΙ ΜΩΡΗ ΑΝΑΠΗΡΗ!!!!!!!!!" << endl;
//    
//    auto avxTest = STLKR_Tests::RawPointerAVX_PerformanceTest<4000>(exportPath);
//    //auto avxTest = STLKR_Tests::RawPointerAVX_PerformanceTest<40000000>(exportPath);
//
//    auto timer = std::chrono::high_resolution_clock::now();
//    for (int i = 0; i < 1; i++) {
//        //avxTest.runTest();
//        //avxTest.runPrefetchDistancePerformanceTest();
//        avxTest.runHintPerformanceTest();
//        
//        
//        cout << "Performance Test " << i + 1 << " completed" << endl;
//        //std::this_thread::sleep_for(std::chrono::milliseconds(500));
//        //cout<<"He not like us, he not like us, he not like us" << endl;
//    }
//    auto end = std::chrono::high_resolution_clock::now();
//    auto duration = std::chrono::duration_cast<std::chrono::hours>(end - timer).count();
//    cout << "Total time taken: " << duration << " h" << endl;
    
    std::cout << "Hello, World!" << std::endl;


    //STLKR_LinearAlgebra::STLKR_SIMD_Operations<double>::add(data1, data2, result, 4);
    //LinearAlgebra::NumericalVector<double> vector1 = LinearAlgebra::NumericalVector<double>(3);
    return 0;
}
