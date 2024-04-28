//
// Created by hal9000 on 4/27/24.
//

#ifndef STALKER_AVX_PERFORMANCETEST_H
#define STALKER_AVX_PERFORMANCETEST_H
#include "../LinearAlgebra/ContiguousMemoryStorage/STLKR_EnumerableMathematicalOperations/STLKR_Operations_SIMD.h"
#include <chrono>
#include <iostream>

namespace STLKR_Tests{
    class AVX_PerformanceTest {
    public:
        static void RunPerformanceTestCopy(){
            const int size = 1000000;

            // Use _mm_malloc to allocate memory aligned to 32 bytes for AVX
            auto destinationAVX = static_cast<double*>(_mm_malloc(size * sizeof(double), 32));
            auto destinationNoAVX = new double[size]; // Standard allocation for comparison
            auto source = static_cast<double*>(_mm_malloc(size * sizeof(double), 32));
            
            for (int i = 0; i < size; i++) {
                source[i] = i;
            }
            auto start = std::chrono::high_resolution_clock::now();
            STLKR_Operations_SIMD<double, 4, 2>::deepcopy(destinationAVX, source);
            //end timer
            auto end = std::chrono::high_resolution_clock::now();
            //duration in microseconds
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
            cout << "elapsed time with AVX: " << duration.count() << "nanos\n";
            
            start = std::chrono::high_resolution_clock::now();
            deepCopyNoAVX(destinationNoAVX, source, size);
            //end timer
            end = std::chrono::high_resolution_clock::now();
            //duration in microseconds
            duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
            cout << "elapsed time without AVX: " << duration.count() << "nanos\n";
            
            delete[] destinationAVX;
            delete[] destinationNoAVX;
            delete[] source;
            
        }

        static void RunPerformanceTestAdd(){
            const int size = 40000000;

            // Use _mm_malloc to allocate memory aligned to 32 bytes for AVX
            auto data1 = static_cast<double*>(_mm_malloc(size * sizeof(double), 32));
            auto data2 = static_cast<double*>(_mm_malloc(size * sizeof(double), 32));
            auto resultAVX = static_cast<double*>(_mm_malloc(size * sizeof(double), 32));
            auto resultNoAVX = new double[size]; // Standard allocation for comparison
            
            for (int i = 0; i < size; i++) {
                data1[i] = i;
                data2[i] = i;
            }
            
            auto start = std::chrono::high_resolution_clock::now();
            STLKR_Operations_SIMD<double, size, 2>::add(data1, 1, data2, 1, resultAVX);
            auto end = std::chrono::high_resolution_clock::now();
            //duration in microseconds
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            cout << "AVX [ENABLED ] " << duration.count() << " ms\n";

            start = std::chrono::high_resolution_clock::now();
            addNoAVX(data1, 1, data2, 1, resultNoAVX, size);
            //end timer
            end = std::chrono::high_resolution_clock::now();
            //duration in microseconds
            duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            cout << "AVX [DISABLED] " << duration.count() << " ms\n";
            
            //check results
            for (int i = 0; i < size; i++) {
                if (resultAVX[i] != resultNoAVX[i]) {
                    cout << "ERROR: resultAVX[" << i << "] = " << resultAVX[i] << " | resultNoAVX[" << i << "] = " << resultNoAVX[i] << "\n";
                }
               //cout << "resultAVX[" << i << "] = " << resultAVX[i] << " | resultNoAVX[" << i << "] = " << resultNoAVX[i] << "\n";
            }
            delete[] data1;
            delete[] data2;
            delete[] resultAVX;
            delete[] resultNoAVX;

        }
    private:
        static void addNoAVX(double* data1, double c1, double* data2, double c2, double* result, int size){
            for (int i = 0; i < size; i++) {
                result[i] = data1[i] * c1 + data2[i] * c2;
            }
        }
        
        static void deepCopyNoAVX(double* destination, double* source, int size){
            for (int i = 0; i < size; i++) {
                destination[i] = source[i];
            }
        }
};
}

#endif //STALKER_AVX_PERFORMANCETEST_H
