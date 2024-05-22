//
// Created by hal9000 on 4/27/24.
//

#ifndef STALKER_AVX_PERFORMANCETEST_H
#define STALKER_AVX_PERFORMANCETEST_H

#include <cassert>
#include <thread>
#include "../../LinearAlgebra/ContiguousMemoryStorage/STLKR_EnumerableMathematicalOperations/STLKR_Operations_SIMD.h"
#include "STLKR_PerformanceTestBase.h"


//Notes: Unroll 2 is one order of magnitude slower than unroll 4.
//       No fetch is faster for 32 and 64 byte alignment. same with fetch for no alignment.
//
namespace STLKR_Tests{
    template<size_t size>
    class RawPointerAVX_PerformanceTest : public STLKR_PerformanceTestBase{
    public:
        
        explicit RawPointerAVX_PerformanceTest(const string &path) : STLKR_PerformanceTestBase("RawPointerAVX_PerformanceTest", path) {}
        
        void runTest() override {
            //static_assert(size % 4 == 0, "Size must be a multiple of 4");
            //runAlignmentSizePerformanceTest(32);
            runPrefetchDistancePerformanceTest();
            //runHintPerformanceTest();
            //runHintPerformanceTestDotProduct();
        }
        void runAlignmentSizePerformanceTest(){
            logs.addComment("AVX Addition Performance Test for alignment size");
            logs.addComment("Operations are performed with and without prefetching");

            logs.addParameter("threads:", 1);
            logs.addParameter("size:", static_cast<double>(size));
            logs.addParameter("alignment_1:", 32);
            logs.addParameter("alignment_2:", 64);
            logs.addParameter("alignment_3:", "none");
            logs.addParameter("prefetch_1:", "true");
            logs.addParameter("prefetch_2:", "false");

            _runAlignmentSizeAddition(64);
            _runAlignmentSizeAddition(32);
            _runAlignmentSizeAddition();

            logs.exportToCSV(_path + "/AlignmentSize", "RawPointerAVX_PerformanceTest_AlignmentSize");
            logs.clearAllLogs();
        }

        void runPrefetchDistancePerformanceTest(){
            logs.addComment("AVX Addition Performance Test prefetch distance");
            logs.addComment("Operations are performed with and without prefetching");

            logs.addParameter("threads:", 1);
            logs.addParameter("size:", static_cast<double>(size));
            logs.addParameter("alignment:", 64);
            logs.addParameter("prefetch_1:", "true");
            logs.addParameter("prefetch_distance_3:", 64);
            logs.addParameter("prefetch_distance_4:", 128);
            logs.addParameter("prefetch_distance_5:", 256);

            _runPrefetchDistance();

            logs.exportToCSV(_path + "/PrefetchDistance", "RawPointerAVX_PerformanceTest_PrefetchDistance");
            logs.clearAllLogs();
        }

        void runHintPerformanceTest(){


            logs.addComment("AVX Addition Performance Test avx hints");

            logs.addParameter("threads:", 1);
            logs.addParameter("size:", static_cast<double>(size));
            logs.addParameter("alignment:", 64);
            logs.addParameter("prefetch_1:", "true");
            logs.addParameter("prefetch_distance_1:", 64);
            logs.addParameter("prefetch_distance_2:", 128);
            logs.addParameter("prefetch_distance_3:", 256);
            logs.addParameter("hint_1:", "T0");
            logs.addParameter("hint_2:", "T1");
            logs.addParameter("hint_3:", "T2");


            _runHints2();

            logs.exportToCSV(_path + "/Hints", "RawPointerAVX_PerformanceTest_Hints");
            logs.clearAllLogs();
        }
    
    private:
        


        void runHintPerformanceTestDotProduct(){
            logs.addComment("AVX Addition Performance Test avx hints dot product");

            logs.addParameter("threads:", 1);
            logs.addParameter("size:", static_cast<double>(size));
            logs.addParameter("alignment:", 64);
            logs.addParameter("prefetch_1:", "true");
            logs.addParameter("prefetch_distance_1:", 64);
            logs.addParameter("prefetch_distance_2:", 128);
            logs.addParameter("prefetch_distance_3:", 256);
            logs.addParameter("hint_1:", "T0");
            logs.addParameter("hint_2:", "T1");
            logs.addParameter("hint_3:", "T2");


            _runHints();

            logs.exportToCSV(_path + "/Hints", "RawPointerAVX_PerformanceTest_Hints");
            logs.clearAllLogs();
        }
        
        constexpr void _runAlignmentSizeAddition(size_t alignmentSize = 0){
            double* data1;
            double* data2;
            double* resultAVX;
            double* resultNoAVX;
            if (alignmentSize == 0){
                data1 = new double[size];
                data2 = new double[size];
                resultAVX = new double[size];
                resultNoAVX = new double[size];
            } else {
                data1 = static_cast<double*>(_mm_malloc(size * sizeof(double), alignmentSize));
                data2 = static_cast<double*>(_mm_malloc(size * sizeof(double), alignmentSize));
                resultAVX = static_cast<double*>(_mm_malloc(size * sizeof(double), alignmentSize));
                resultNoAVX = static_cast<double*>(_mm_malloc(size * sizeof(double), alignmentSize));
            }

            for (int i = 0; i < size; i++) {
                data1[i] = i;
                data2[i] = i;
            }
            if (alignmentSize != 0){
                logs.startSingleObservationTimer("add_avx_on_align_" + std::to_string(alignmentSize) + "_unroll_2_prefetch", STLKR_TimeUnit::microseconds);
                STLKR_Operations_SIMD<double, size, 2>::add_unroll2_prefetch(data1, 1, data2, 1, resultAVX);
                logs.stopSingleObservationTimer("add_avx_on_align_" + std::to_string(alignmentSize) + "_unroll_2_prefetch");

                logs.startSingleObservationTimer("add_avx_on_align_" + std::to_string(alignmentSize) + "_unroll_4_prefetch", STLKR_TimeUnit::microseconds);
                STLKR_Operations_SIMD<double, size, 2>::add_unroll4_prefetch(data1, 1, data2, 1, resultAVX);
                logs.stopSingleObservationTimer("add_avx_on_align_" + std::to_string(alignmentSize) + "_unroll_4_prefetch");

                logs.startSingleObservationTimer("add_avx_on_align_" + std::to_string(alignmentSize) + "_unroll_2_no_prefetch", STLKR_TimeUnit::microseconds);
                STLKR_Operations_SIMD<double, size, 2>::add_unroll2_noPrefetch(data1, 1, data2, 1, resultAVX);
                logs.stopSingleObservationTimer("add_avx_on_align_" + std::to_string(alignmentSize) + "_unroll_2_no_prefetch");

                logs.startSingleObservationTimer("add_avx_on_align_" + std::to_string(alignmentSize) + "_unroll_4_no_prefetch", STLKR_TimeUnit::microseconds);
                STLKR_Operations_SIMD<double, size, 2>::add_unroll4_noPrefetch(data1, 1, data2, 1, resultAVX);
                logs.stopSingleObservationTimer("add_avx_on_align_" + std::to_string(alignmentSize) + "_unroll_4_no_prefetch");
            }

            logs.startSingleObservationTimer("add_avx_off_align_" + std::to_string(alignmentSize) + "_unroll_2", STLKR_TimeUnit::microseconds);
            addUnroll2NoAVX(data1, 1, data2, 1, resultNoAVX);
            logs.stopSingleObservationTimer("add_avx_off_align_" + std::to_string(alignmentSize) + "_unroll_2");

            logs.startSingleObservationTimer("add_avx_off_align_" + std::to_string(alignmentSize) + "_unroll_4", STLKR_TimeUnit::microseconds);
            addUnroll4NoAVX(data1, 1, data2, 1, resultNoAVX);
            logs.stopSingleObservationTimer("add_avx_off_align_" + std::to_string(alignmentSize) + "_unroll_4");

            //logs.storeAndResetCurrentLogs();
            if (alignmentSize != 0){
                _mm_free(data1);
                _mm_free(data2);
                _mm_free(resultAVX);
                _mm_free(resultNoAVX);
            } else {
                delete[] data1;
                delete[] data2;
                delete[] resultAVX;
                delete[] resultNoAVX;
            }

        }

        constexpr void _runPrefetchDistance(){
            double* data1;
            double* data2;
            double* resultAVX;

            data1 = static_cast<double*>(_mm_malloc(size * sizeof(double), 64));
            data2 = static_cast<double*>(_mm_malloc(size * sizeof(double), 64));
            resultAVX = static_cast<double*>(_mm_malloc(size * sizeof(double), 64));

            assert(reinterpret_cast<uintptr_t>(data1) % 64 == 0);
            assert(reinterpret_cast<uintptr_t>(data2) % 64 == 0);
            assert(reinterpret_cast<uintptr_t>(resultAVX) % 64 == 0);

            for (int i = 0; i < size; i++) {
                data1[i] = i;
                data2[i] = i;
            }

            STLKR_SIMD_Prefetch_Config prefetchConfig;
            prefetchConfig.hint = STLKR_SIMD_Prefetch_Hint::T0;
            prefetchConfig.storeType = STLKR_SIMD_Stores::Regular;
            
            prefetchConfig.distance = STLKR_SIMD_Prefetch_Distance::_64;
            logs.startSingleObservationTimer("add_avx_on_unroll_16_prefetch_64", STLKR_TimeUnit::nanoseconds);
            STLKR_Operations_SIMD<double, size, 2>::add(data1, 1, data2, 1, resultAVX, prefetchConfig);
            logs.stopSingleObservationTimer("add_avx_on_unroll_16_prefetch_64");
            
            prefetchConfig.distance = STLKR_SIMD_Prefetch_Distance::_128;
            logs.startSingleObservationTimer("add_avx_on_unroll_16_prefetch_128", STLKR_TimeUnit::nanoseconds);
            STLKR_Operations_SIMD<double, size, 2>::add(data1, 1, data2, 1, resultAVX, prefetchConfig);
            logs.stopSingleObservationTimer("add_avx_on_unroll_16_prefetch_128");
            
            prefetchConfig.distance = STLKR_SIMD_Prefetch_Distance::_256;
            logs.startSingleObservationTimer("add_avx_on_unroll_16_prefetch_256", STLKR_TimeUnit::nanoseconds);
            STLKR_Operations_SIMD<double, size, 2>::add(data1, 1, data2, 1, resultAVX, prefetchConfig);
            logs.stopSingleObservationTimer("add_avx_on_unroll_16_prefetch_256");
            
            logs.startSingleObservationTimer("add_avx_off_unroll_16", STLKR_TimeUnit::nanoseconds);
            addNoAVX(data1, 1, data2, 1, resultAVX);
            logs.stopSingleObservationTimer("add_avx_off_unroll_16");
            
            
            _mm_free(data1);
            _mm_free(data2);
            _mm_free(resultAVX);
        }

        constexpr void _runHints(){
            double* data1;
            double* data2;
            double* resultAVX;

            data1 = static_cast<double*>(_mm_malloc(size * sizeof(double), 64));
            data2 = static_cast<double*>(_mm_malloc(size * sizeof(double), 64));
            resultAVX = static_cast<double*>(_mm_malloc(size * sizeof(double), 64));

            assert(reinterpret_cast<uintptr_t>(data1) % 64 == 0);
            assert(reinterpret_cast<uintptr_t>(data2) % 64 == 0);
            assert(reinterpret_cast<uintptr_t>(resultAVX) % 64 == 0);

            for (int i = 0; i < size; i++) {
                data1[i] = i;
                data2[i] = i;
            }

            STLKR_SIMD_Prefetch_Config prefetchConfig;
            prefetchConfig.storeType = STLKR_SIMD_Stores::NonTemporal;

            prefetchConfig.distance = STLKR_SIMD_Prefetch_Distance::_64;
            prefetchConfig.hint = STLKR_SIMD_Prefetch_Hint::T0;
            logs.startSingleObservationTimer("add_avx_on_unroll_16_prefetch_64_hint_t0", STLKR_TimeUnit::nanoseconds);
            STLKR_Operations_SIMD<double, size, 2>::add(data1, 1, data2, 1, resultAVX, prefetchConfig);
            logs.stopSingleObservationTimer("add_avx_on_unroll_16_prefetch_64_hint_t0");

            prefetchConfig.hint = STLKR_SIMD_Prefetch_Hint::T1;
            logs.startSingleObservationTimer("add_avx_on_unroll_16_prefetch_64_hint_t1", STLKR_TimeUnit::nanoseconds);
            STLKR_Operations_SIMD<double, size, 2>::add(data1, 1, data2, 1, resultAVX, prefetchConfig);
            logs.stopSingleObservationTimer("add_avx_on_unroll_16_prefetch_64_hint_t1");

            prefetchConfig.hint = STLKR_SIMD_Prefetch_Hint::T2;
            logs.startSingleObservationTimer("add_avx_on_unroll_16_prefetch_64_hint_t2", STLKR_TimeUnit::nanoseconds);
            STLKR_Operations_SIMD<double, size, 2>::add(data1, 1, data2, 1, resultAVX, prefetchConfig);
            logs.stopSingleObservationTimer("add_avx_on_unroll_16_prefetch_64_hint_t2");

            prefetchConfig.distance = STLKR_SIMD_Prefetch_Distance::_64;
            prefetchConfig.hint = STLKR_SIMD_Prefetch_Hint::T0;
            logs.startSingleObservationTimer("add_avx_on_unroll_16_prefetch_64_hint_t0", STLKR_TimeUnit::nanoseconds);
            STLKR_Operations_SIMD<double, size, 2>::add(data1, 1, data2, 1, resultAVX, prefetchConfig);
            logs.stopSingleObservationTimer("add_avx_on_unroll_16_prefetch_64_hint_t0");

            prefetchConfig.hint = STLKR_SIMD_Prefetch_Hint::T1;
            logs.startSingleObservationTimer("add_avx_on_unroll_16_prefetch_64_hint_t1", STLKR_TimeUnit::nanoseconds);
            STLKR_Operations_SIMD<double, size, 2>::add(data1, 1, data2, 1, resultAVX, prefetchConfig);
            logs.stopSingleObservationTimer("add_avx_on_unroll_16_prefetch_64_hint_t1");

            prefetchConfig.hint = STLKR_SIMD_Prefetch_Hint::T2;
            logs.startSingleObservationTimer("add_avx_on_unroll_16_prefetch_64_hint_t2", STLKR_TimeUnit::nanoseconds);
            STLKR_Operations_SIMD<double, size, 2>::add(data1, 1, data2, 1, resultAVX, prefetchConfig);
            logs.stopSingleObservationTimer("add_avx_on_unroll_16_prefetch_64_hint_t2");

            prefetchConfig.distance = STLKR_SIMD_Prefetch_Distance::_128;
            prefetchConfig.hint = STLKR_SIMD_Prefetch_Hint::T0;
            logs.startSingleObservationTimer("add_avx_on_unroll_16_prefetch_128_hint_t0", STLKR_TimeUnit::nanoseconds);
            STLKR_Operations_SIMD<double, size, 2>::add(data1, 1, data2, 1, resultAVX, prefetchConfig);
            logs.stopSingleObservationTimer("add_avx_on_unroll_16_prefetch_128_hint_t0");

            prefetchConfig.hint = STLKR_SIMD_Prefetch_Hint::T1;
            logs.startSingleObservationTimer("add_avx_on_unroll_16_prefetch_128_hint_t1", STLKR_TimeUnit::nanoseconds);
            STLKR_Operations_SIMD<double, size, 2>::add(data1, 1, data2, 1, resultAVX, prefetchConfig);
            logs.stopSingleObservationTimer("add_avx_on_unroll_16_prefetch_128_hint_t1");

            prefetchConfig.hint = STLKR_SIMD_Prefetch_Hint::T2;
            logs.startSingleObservationTimer("add_avx_on_unroll_16_prefetch_128_hint_t2", STLKR_TimeUnit::nanoseconds);
            STLKR_Operations_SIMD<double, size, 2>::add(data1, 1, data2, 1, resultAVX, prefetchConfig);
            logs.stopSingleObservationTimer("add_avx_on_unroll_16_prefetch_128_hint_t2");


            prefetchConfig.distance = STLKR_SIMD_Prefetch_Distance::_256;
            prefetchConfig.hint = STLKR_SIMD_Prefetch_Hint::T0;
            logs.startSingleObservationTimer("add_avx_on_unroll_16_prefetch_256_hint_t0", STLKR_TimeUnit::nanoseconds);
            STLKR_Operations_SIMD<double, size, 2>::add(data1, 1, data2, 1, resultAVX, prefetchConfig);
            logs.stopSingleObservationTimer("add_avx_on_unroll_16_prefetch_256_hint_t0");

            prefetchConfig.hint = STLKR_SIMD_Prefetch_Hint::T1;
            logs.startSingleObservationTimer("add_avx_on_unroll_16_prefetch_256_hint_t1", STLKR_TimeUnit::nanoseconds);
            STLKR_Operations_SIMD<double, size, 2>::add(data1, 1, data2, 1, resultAVX, prefetchConfig);
            logs.stopSingleObservationTimer("add_avx_on_unroll_16_prefetch_256_hint_t1");

            prefetchConfig.hint = STLKR_SIMD_Prefetch_Hint::T2;
            logs.startSingleObservationTimer("add_avx_on_unroll_16_prefetch_256_hint_t2", STLKR_TimeUnit::nanoseconds);
            STLKR_Operations_SIMD<double, size, 2>::add(data1, 1, data2, 1, resultAVX, prefetchConfig);
            logs.stopSingleObservationTimer("add_avx_on_unroll_16_prefetch_256_hint_t2");
            
            logs.startSingleObservationTimer("add_avx_off_unroll_0", STLKR_TimeUnit::nanoseconds);
            addNoAVX2(data1, 1, data2, 1, resultAVX);
            logs.stopSingleObservationTimer("add_avx_off_unroll_0");
            
            _mm_free(data1);
            _mm_free(data2);
            _mm_free(resultAVX);
        }

        constexpr void _runHints2(){
            double* data1;
            double* data2;
            double* resultAVX;

            data1 = static_cast<double*>(_mm_malloc(size * sizeof(double), 64));
            data2 = static_cast<double*>(_mm_malloc(size * sizeof(double), 64));
            resultAVX = static_cast<double*>(_mm_malloc(size * sizeof(double), 64));

            assert(reinterpret_cast<uintptr_t>(data1) % 64 == 0);
            assert(reinterpret_cast<uintptr_t>(data2) % 64 == 0);
            assert(reinterpret_cast<uintptr_t>(resultAVX) % 64 == 0);

//            for (int i = 0; i < size; i++) {
//                data1[i] = 1;
//                data2[i] = 2;
//            }

            for (int i = 0; i < size; i++) {
                data1[i] = 1;
                data2[i] = 1;
            }
            
            double* data[2] = {data1, data2};
            double coefficientsArray[2] = {1,0};

            STLKR_SIMD_Prefetch_Config prefetchConfig;
            prefetchConfig.storeType = STLKR_SIMD_Stores::NonTemporal;

            prefetchConfig.distance = STLKR_SIMD_Prefetch_Distance::_64;
            prefetchConfig.hint = STLKR_SIMD_Prefetch_Hint::T0;
            logs.startSingleObservationTimer("add_avx_on_unroll_16_prefetch_64_hint_t0", STLKR_TimeUnit::nanoseconds);
            STLKR_Operations_SIMD<double, size, 4>::template addUnrolled<2>(data, coefficientsArray, resultAVX, prefetchConfig);
            logs.stopSingleObservationTimer("add_avx_on_unroll_16_prefetch_64_hint_t0");
            

            logs.startSingleObservationTimer("add_avx_off_unroll_0", STLKR_TimeUnit::nanoseconds);
            addNoAVX2(data1, 1, data2, 1, resultAVX);
            logs.stopSingleObservationTimer("add_avx_off_unroll_0");

            _mm_free(data1);
            _mm_free(data2);
            _mm_free(resultAVX);
        }
        
        static constexpr inline void addNoAVX(const double* data1, double c1, const double* data2, double c2, double* result){
            for (int i = 0; i < size; i ++) {
                result[i] = data1[i] * c1 + data2[i] * c2;
            }
        }

        static void addNoAVX2(const double* data1, double c1, const double* data2, double c2, double* result){
            for (int i = 0; i < size; i ++) {
                result[i] = data1[i] * c1 + data2[i] * c2;
            }
        }
        
        static constexpr inline void addUnroll2NoAVX(const double* data1, double c1, const double* data2, double c2, double* result){
            for (int i = 0; i < size; i += 2) {
                result[i] = data1[i] * c1 + data2[i] * c2;
                result[i + 1] = data1[i + 1] * c1 + data2[i + 1] * c2;
            }
        }

        static constexpr inline void addUnroll4NoAVX(const double* data1, double c1, const double* data2, double c2, double* result){
            for (int i = 0; i < size; i += 4) {
                result[i] = data1[i] * c1 + data2[i] * c2;
                result[i + 1] = data1[i + 1] * c1 + data2[i + 1] * c2;
                result[i + 2] = data1[i + 2] * c1 + data2[i + 2] * c2;
                result[i + 3] = data1[i + 3] * c1 + data2[i + 3] * c2;
            }
        }

        static constexpr inline void addUnroll8NoAVX(const double* data1, double c1, const double* data2, double c2, double* result){
            for (int i = 0; i < size; i += 4) {
                result[i] = data1[i] * c1 + data2[i] * c2;
                result[i + 1] = data1[i + 1] * c1 + data2[i + 1] * c2;
                result[i + 2] = data1[i + 2] * c1 + data2[i + 2] * c2;
                result[i + 3] = data1[i + 3] * c1 + data2[i + 3] * c2;
                result[i + 4] = data1[i + 4] * c1 + data2[i + 4] * c2;
                result[i + 5] = data1[i + 5] * c1 + data2[i + 5] * c2;
                result[i + 6] = data1[i + 6] * c1 + data2[i + 6] * c2;
                result[i + 7] = data1[i + 7] * c1 + data2[i + 7] * c2;
            }
        }

        static constexpr inline void addUnroll16NoAVX(const double* data1, double c1, const double* data2, double c2, double* result){
            for (int i = 0; i < size; i += 4) {
                result[i] = data1[i] * c1 + data2[i] * c2;
                result[i + 1] = data1[i + 1] * c1 + data2[i + 1] * c2;
                result[i + 2] = data1[i + 2] * c1 + data2[i + 2] * c2;
                result[i + 3] = data1[i + 3] * c1 + data2[i + 3] * c2;
                result[i + 4] = data1[i + 4] * c1 + data2[i + 4] * c2;
                result[i + 5] = data1[i + 5] * c1 + data2[i + 5] * c2;
                result[i + 6] = data1[i + 6] * c1 + data2[i + 6] * c2;
                result[i + 7] = data1[i + 7] * c1 + data2[i + 7] * c2;
                result[i + 8] = data1[i + 8] * c1 + data2[i + 8] * c2;
                result[i + 9] = data1[i + 9] * c1 + data2[i + 9] * c2;
                result[i + 10] = data1[i + 10] * c1 + data2[i + 10] * c2;
                result[i + 11] = data1[i + 11] * c1 + data2[i + 11] * c2;
                result[i + 12] = data1[i + 12] * c1 + data2[i + 12] * c2;
                result[i + 13] = data1[i + 13] * c1 + data2[i + 13] * c2;
                result[i + 14] = data1[i + 14] * c1 + data2[i + 14] * c2;
                result[i + 15] = data1[i + 15] * c1 + data2[i + 15] * c2;
            }
        }
        
        static void deepCopyNoAVX(double* destination, const double* source){
            for (int i = 0; i < size; i++) {
                destination[i] = source[i];
            }
        }
};
}

#endif //STALKER_AVX_PERFORMANCETEST_H
