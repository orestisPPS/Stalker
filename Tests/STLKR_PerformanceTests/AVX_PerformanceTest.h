//
// Created by hal9000 on 4/27/24.
//

#ifndef STALKER_AVX_PERFORMANCETEST_H
#define STALKER_AVX_PERFORMANCETEST_H
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
            
            logs.addComment("AVX Addition Performance Test");
            logs.addComment("Operations are performed with and without prefetching");
            
            logs.addParameter("threads:", 1);
            logs.addParameter("size:", static_cast<double>(size));
            logs.addParameter("alignment_1:", 32);
            logs.addParameter("alignment_2:", 64);
            logs.addParameter("alignment_3:", "none");
            logs.addParameter("prefetch_1:", "true");
            logs.addParameter("prefetch_2:", "false");
            
            checkAlignmentSizeAddition(32);
            checkAlignmentSizeAddition(64);
            //checkAlignmentSizeAddition();
            
            logs.exportToCSV(_path , "RawPointerAVX_PerformanceTest");
        }
        
        constexpr void checkAlignmentSizeAddition(size_t alignmentSize = 0){
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
            logs.startSingleObservationTimer("avx_addition_alignment_" + std::to_string(alignmentSize) + "_unroll2_prefetch", STLKR_TimeUnit::microseconds);
            STLKR_Operations_SIMD<double, size, 2>::add_unroll2_prefetch(data1, 1, data2, 1, resultAVX);
            logs.stopSingleObservationTimer("avx_addition_alignment_" + std::to_string(alignmentSize) + "_unroll2_prefetch");

            logs.startSingleObservationTimer("avx_addition_alignment_" + std::to_string(alignmentSize) + "_unroll4_prefetch", STLKR_TimeUnit::microseconds);
            STLKR_Operations_SIMD<double, size, 2>::add_unroll4_prefetch(data1, 1, data2, 1, resultAVX);
            logs.stopSingleObservationTimer("avx_addition_alignment_" + std::to_string(alignmentSize) + "_unroll4_prefetch");

            logs.startSingleObservationTimer("avx_addition_alignment_" + std::to_string(alignmentSize) + "_unroll2_no_prefetch", STLKR_TimeUnit::microseconds);
            STLKR_Operations_SIMD<double, size, 2>::add_unroll2_noPrefetch(data1, 1, data2, 1, resultAVX);
            logs.stopSingleObservationTimer("avx_addition_alignment_" + std::to_string(alignmentSize) + "_unroll2_no_prefetch");

            logs.startSingleObservationTimer("avx_addition_alignment_" + std::to_string(alignmentSize) + "_unroll4_no_prefetch", STLKR_TimeUnit::microseconds);
            STLKR_Operations_SIMD<double, size, 2>::add_unroll4_noPrefetch(data1, 1, data2, 1, resultAVX);
            logs.stopSingleObservationTimer("avx_addition_alignment_" + std::to_string(alignmentSize) + "_unroll4_no_prefetch");

            logs.startSingleObservationTimer("no_avx_addition_alignment_" + std::to_string(alignmentSize) + "_unroll2", STLKR_TimeUnit::microseconds);
            addUnroll2NoAVX(data1, 1, data2, 1, resultNoAVX);
            logs.stopSingleObservationTimer("no_avx_addition_alignment_" + std::to_string(alignmentSize) + "_unroll2");

            logs.startSingleObservationTimer("no_avx_addition_alignment_" + std::to_string(alignmentSize) + "_unroll4", STLKR_TimeUnit::microseconds);
            addUnroll4NoAVX(data1, 1, data2, 1, resultNoAVX);
            logs.stopSingleObservationTimer("no_avx_addition_alignment_" + std::to_string(alignmentSize) + "_unroll4");

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
   
    private:
        
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
        
        static void deepCopyNoAVX(double* destination, const double* source){
            for (int i = 0; i < size; i++) {
                destination[i] = source[i];
            }
        }
};
}

#endif //STALKER_AVX_PERFORMANCETEST_H
