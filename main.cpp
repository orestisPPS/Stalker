#include <iostream>
#include "Tests/STLKR_PerformanceTests/AVX_PerformanceTest.h"
#include "Tests/STLKR_PerformanceTests/ThreadingPerformanceTest.h"
#include "LinearAlgebra/ContiguousMemoryStorage/NumericalVector.h"
#include <chrono>

int main() {
    
    auto exportPath = "../Tests/STLKR_PerformanceTests/logs/Threading";
    auto threadingTest = STLKR_Tests::ThreadingPerformanceTest<400000, 2>(exportPath);

    for (int i = 0; i < 10; i++) {
        threadingTest.runTest();
    }


 
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
    


    //STLKR_LinearAlgebra::STLKR_SIMD_Operations<double>::add(data1, data2, result, 4);
    //LinearAlgebra::NumericalVector<double> vector1 = LinearAlgebra::NumericalVector<double>(3);
    return 0;
}
