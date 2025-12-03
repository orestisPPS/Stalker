#pragma once
#include "STLKR_TestBase.h"
#include <Stalker/Mathematics/Matrix/MatrixMath.h>
#include <cmath>

namespace STLKR_Tests {

    using namespace Stalker::Mathematics;
    using namespace Stalker::Memory;

    class MatrixMathTests : public STLKR_TestBase {
    public:

    explicit MatrixMathTests(size_t size = 10000, double tolerance = 1e-1)  
                : STLKR_TestBase("MatrixMathTests"), _size(size), _tolerance(tolerance) {
            setPath(_testName + "Logs", getPath(_testName) + "/logs/" + _testName);
            Random::setSeed(42); // Set a fixed seed for reproducibility
        }
    
    void runTest() override {
        printTitle("Matrix Math Operations Validity Tests", "-", T_Color::WHITE);
        
        // _testAdd<double>();
        // _testAdd<float>();
        // _testAdd<int>();
        
        // _testSubtract<double>();
        // _testSubtract<float>();
        // _testSubtract<int>();

        // _testMultiply<double>();
        // _testMultiply<float>();
        // _testMultiply<int>();

        // _testSum<double>();
        // _testSum<float>();
        // _testSum<int>();

        // _testScale<double>();
        // _testScale<float>();
        // _testScale<int>();

        // _testAddConstant<double>();
        // _testAddConstant<float>();
        // _testAddConstant<int>();

        _testMatrixVectorMultiply<double>();
        _testMatrixVectorMultiply<float>();
        _testMatrixVectorMultiply<int>();

        // _testVectorMatrixMultiply<double>();
        // _testVectorMatrixMultiply<float>();
        // _testVectorMatrixMultiply<int>();
    }

    private:

    size_t _size;
    double _tolerance;

    template<typename T>
    void _testAdd() {
        printSubtitle("Matrix Add " + TestUtility::getTypeString<T>(), T_Color::BARBIE_PINK);
        
        size_t rows = static_cast<size_t>(std::sqrt(_size));
        size_t cols = rows;
        size_t total = rows * cols;

        auto a = createAlignedVector<T>(total);
        auto b = createAlignedVector<T>(total);
        Random::uniform<T>(total, a.data(), 0, 100);
        Random::uniform<T>(total, b.data(), 0, 100);
        
        auto expected = createAlignedVector<T>(total);
        for(size_t i=0; i<total; ++i) expected[i] = a[i] + b[i];

        // Classic
        {
            auto res = createAlignedVector<T>(total);
            MatrixMath::add<T, ExecutionTraitClassic<>>(rows, cols, a.data(), b.data(), res.data());
            TestUtility::compareVectors(res.data(), expected.data(), total, "Classic", _tolerance);
        }

        // Meta
        {
            auto res = createAlignedVector<T>(total);
            MatrixMath::add<T, ExecutionTraitUnrolled<>>(rows, cols, a.data(), b.data(), res.data());
            TestUtility::compareVectors(res.data(), expected.data(), total, "Meta", _tolerance);
        }

        #if defined(STALKER_SIMD_AVX2_OK)
        {
            auto res = createAlignedVector<T>(total);
            MatrixMath::add<T, ExecutionTraitSIMD<T_SIMD::AVX2>>(rows, cols, a.data(), b.data(), res.data());
            TestUtility::compareVectors(res.data(), expected.data(), total, "SIMD AVX2", _tolerance);
        }
        #endif

        #if defined(STALKER_SIMD_AVX512_OK)
        {
            auto res = createAlignedVector<T>(total);
            MatrixMath::add<T, ExecutionTraitSIMD<T_SIMD::AVX512>>(rows, cols, a.data(), b.data(), res.data());
            TestUtility::compareVectors(res.data(), expected.data(), total, "SIMD AVX512", _tolerance);
        }
        #endif

        #if defined(STALKER_THREADING_ENABLE) && STALKER_THREADING_ENABLE != 0
        ThreadTraitSTDThread threadTrait;
        
        {
            auto res = createAlignedVector<T>(total);
            MatrixMath::add<T, ThreadTraitSTDThread, ExecutionTraitClassic<>>(threadTrait, rows, cols, a.data(), b.data(), res.data());
            TestUtility::compareVectors(res.data(), expected.data(), total, "Threaded Classic", _tolerance);
        }

        {
            auto res = createAlignedVector<T>(total);
            MatrixMath::add<T, ThreadTraitSTDThread, ExecutionTraitUnrolled<>>(threadTrait, rows, cols, a.data(), b.data(), res.data());
            TestUtility::compareVectors(res.data(), expected.data(), total, "Threaded Meta", _tolerance);
        }

        #if defined(STALKER_SIMD_AVX2_OK)
        {
            auto res = createAlignedVector<T>(total);
            MatrixMath::add<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2>>(threadTrait, rows, cols, a.data(), b.data(), res.data());
            TestUtility::compareVectors(res.data(), expected.data(), total, "Threaded SIMD AVX2", _tolerance);
        }
        #endif

        #if defined(STALKER_SIMD_AVX512_OK)
        {
            auto res = createAlignedVector<T>(total);
            MatrixMath::add<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512>>(threadTrait, rows, cols, a.data(), b.data(), res.data());
            TestUtility::compareVectors(res.data(), expected.data(), total, "Threaded SIMD AVX512", _tolerance);
        }
        #endif
        #endif
    }

    template<typename T>
    void _testSubtract() {
        printSubtitle("Matrix Subtract " + TestUtility::getTypeString<T>(), T_Color::BARBIE_PINK);
        
        size_t rows = static_cast<size_t>(std::sqrt(_size));
        size_t cols = rows;
        size_t total = rows * cols;

        auto a = createAlignedVector<T>(total);
        auto b = createAlignedVector<T>(total);
        Random::uniform<T>(total, a.data(), 0, 100);
        Random::uniform<T>(total, b.data(), 0, 100);
        
        auto expected = createAlignedVector<T>(total);
        for(size_t i=0; i<total; ++i) expected[i] = a[i] - b[i];

        // Classic
        {
            auto res = createAlignedVector<T>(total);
            MatrixMath::subtract<T, true, ExecutionTraitClassic<>>(rows, cols, a.data(), b.data(), res.data());
            TestUtility::compareVectors(res.data(), expected.data(), total, "Classic", _tolerance);
        }

        // Meta
        {
            auto res = createAlignedVector<T>(total);
            MatrixMath::subtract<T, true, ExecutionTraitUnrolled<>>(rows, cols, a.data(), b.data(), res.data());
            TestUtility::compareVectors(res.data(), expected.data(), total, "Meta", _tolerance);
        }

        #if defined(STALKER_SIMD_AVX2_OK)
        {
            auto res = createAlignedVector<T>(total);
            MatrixMath::subtract<T, true, ExecutionTraitSIMD<T_SIMD::AVX2>>(rows, cols, a.data(), b.data(), res.data());
            TestUtility::compareVectors(res.data(), expected.data(), total, "SIMD AVX2", _tolerance);
        }
        #endif

        #if defined(STALKER_SIMD_AVX512_OK)
        {
            auto res = createAlignedVector<T>(total);
            MatrixMath::subtract<T, true, ExecutionTraitSIMD<T_SIMD::AVX512>>(rows, cols, a.data(), b.data(), res.data());
            TestUtility::compareVectors(res.data(), expected.data(), total, "SIMD AVX512", _tolerance);
        }
        #endif

        #if defined(STALKER_THREADING_ENABLE) && STALKER_THREADING_ENABLE != 0
        ThreadTraitSTDThread threadTrait;
        
        {
            auto res = createAlignedVector<T>(total);
            MatrixMath::subtract<T, true, ThreadTraitSTDThread, ExecutionTraitClassic<>>(threadTrait, rows, cols, a.data(), b.data(), res.data());
            TestUtility::compareVectors(res.data(), expected.data(), total, "Threaded Classic", _tolerance);
        }

        {
            auto res = createAlignedVector<T>(total);
            MatrixMath::subtract<T, true, ThreadTraitSTDThread, ExecutionTraitUnrolled<>>(threadTrait, rows, cols, a.data(), b.data(), res.data());
            TestUtility::compareVectors(res.data(), expected.data(), total, "Threaded Meta", _tolerance);
        }

        #if defined(STALKER_SIMD_AVX2_OK)
        {
            auto res = createAlignedVector<T>(total);
            MatrixMath::subtract<T, true, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2>>(threadTrait, rows, cols, a.data(), b.data(), res.data());
            TestUtility::compareVectors(res.data(), expected.data(), total, "Threaded SIMD AVX2", _tolerance);
        }
        #endif

        #if defined(STALKER_SIMD_AVX512_OK)
        {
            auto res = createAlignedVector<T>(total);
            MatrixMath::subtract<T, true, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512>>(threadTrait, rows, cols, a.data(), b.data(), res.data());
            TestUtility::compareVectors(res.data(), expected.data(), total, "Threaded SIMD AVX512", _tolerance);
        }
        #endif
        #endif
    }

    template<typename T>
    void _testMultiply() {
        printSubtitle("Matrix Multiply (Element-wise) " + TestUtility::getTypeString<T>(), T_Color::BARBIE_PINK);
        
        size_t rows = static_cast<size_t>(std::sqrt(_size));
        size_t cols = rows;
        size_t total = rows * cols;

        auto a = createAlignedVector<T>(total);
        auto b = createAlignedVector<T>(total);
        Random::uniform<T>(total, a.data(), 0, 100);
        Random::uniform<T>(total, b.data(), 0, 100);
        
        auto expected = createAlignedVector<T>(total);
        for(size_t i=0; i<total; ++i) expected[i] = a[i] * b[i];

        // Classic
        {
            auto res = createAlignedVector<T>(total);
            MatrixMath::multiply<T, ExecutionTraitClassic<>>(rows, cols, a.data(), b.data(), res.data());
            TestUtility::compareVectors(res.data(), expected.data(), total, "Classic", _tolerance);
        }

        // Meta
        {
            auto res = createAlignedVector<T>(total);
            MatrixMath::multiply<T, ExecutionTraitUnrolled<>>(rows, cols, a.data(), b.data(), res.data());
            TestUtility::compareVectors(res.data(), expected.data(), total, "Meta", _tolerance);
        }

        #if defined(STALKER_SIMD_AVX2_OK)
        {
            auto res = createAlignedVector<T>(total);
            MatrixMath::multiply<T, ExecutionTraitSIMD<T_SIMD::AVX2>>(rows, cols, a.data(), b.data(), res.data());
            TestUtility::compareVectors(res.data(), expected.data(), total, "SIMD AVX2", _tolerance);
        }
        #endif

        #if defined(STALKER_SIMD_AVX512_OK)
        {
            auto res = createAlignedVector<T>(total);
            MatrixMath::multiply<T, ExecutionTraitSIMD<T_SIMD::AVX512>>(rows, cols, a.data(), b.data(), res.data());
            TestUtility::compareVectors(res.data(), expected.data(), total, "SIMD AVX512", _tolerance);
        }
        #endif

        #if defined(STALKER_THREADING_ENABLE) && STALKER_THREADING_ENABLE != 0
        ThreadTraitSTDThread threadTrait;
        
        {
            auto res = createAlignedVector<T>(total);
            MatrixMath::multiply<T, ThreadTraitSTDThread, ExecutionTraitClassic<>>(threadTrait, rows, cols, a.data(), b.data(), res.data());
            TestUtility::compareVectors(res.data(), expected.data(), total, "Threaded Classic", _tolerance);
        }

        {
            auto res = createAlignedVector<T>(total);
            MatrixMath::multiply<T, ThreadTraitSTDThread, ExecutionTraitUnrolled<>>(threadTrait, rows, cols, a.data(), b.data(), res.data());
            TestUtility::compareVectors(res.data(), expected.data(), total, "Threaded Meta", _tolerance);
        }

        #if defined(STALKER_SIMD_AVX2_OK)
        {
            auto res = createAlignedVector<T>(total);
            MatrixMath::multiply<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2>>(threadTrait, rows, cols, a.data(), b.data(), res.data());
            TestUtility::compareVectors(res.data(), expected.data(), total, "Threaded SIMD AVX2", _tolerance);
        }
        #endif

        #if defined(STALKER_SIMD_AVX512_OK)
        {
            auto res = createAlignedVector<T>(total);
            MatrixMath::multiply<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512>>(threadTrait, rows, cols, a.data(), b.data(), res.data());
            TestUtility::compareVectors(res.data(), expected.data(), total, "Threaded SIMD AVX512", _tolerance);
        }
        #endif
        #endif
    }

    template<typename T>
    void _testSum() {
        printSubtitle("Matrix Sum " + TestUtility::getTypeString<T>(), T_Color::BARBIE_PINK);
        
        size_t rows = static_cast<size_t>(std::sqrt(_size));
        size_t cols = rows;
        size_t total = rows * cols;

        auto a = createAlignedVector<T>(total);
        Random::uniform<T>(total, a.data(), 0, 10);
        
        T expected = 0;
        for(size_t i=0; i<total; ++i) expected += a[i];

        // Classic
        {
            T res = MatrixMath::sum<T, ExecutionTraitClassic<>>(rows, cols, a.data());
            TestUtility::compareValues(res, expected, "Classic", _tolerance);
        }

        // Meta
        {
            T res = MatrixMath::sum<T, ExecutionTraitUnrolled<>>(rows, cols, a.data());
            TestUtility::compareValues(res, expected, "Meta", _tolerance);
        }

        #if defined(STALKER_SIMD_AVX2_OK)
        {
            T res = MatrixMath::sum<T, ExecutionTraitSIMD<T_SIMD::AVX2>>(rows, cols, a.data());
            TestUtility::compareValues(res, expected, "SIMD AVX2", _tolerance);
        }
        #endif

        #if defined(STALKER_SIMD_AVX512_OK)
        {
            T res = MatrixMath::sum<T, ExecutionTraitSIMD<T_SIMD::AVX512>>(rows, cols, a.data());
            TestUtility::compareValues(res, expected, "SIMD AVX512", _tolerance);
        }
        #endif

        #if defined(STALKER_THREADING_ENABLE) && STALKER_THREADING_ENABLE != 0
        ThreadTraitSTDThread threadTrait;
        
        {
            T res = MatrixMath::sum<T, ThreadTraitSTDThread, ExecutionTraitClassic<>>(threadTrait, rows, cols, a.data());
            TestUtility::compareValues(res, expected, "Threaded Classic", _tolerance);
        }

        {
            T res = MatrixMath::sum<T, ThreadTraitSTDThread, ExecutionTraitUnrolled<>>(threadTrait, rows, cols, a.data());
            TestUtility::compareValues(res, expected, "Threaded Meta", _tolerance);
        }

        #if defined(STALKER_SIMD_AVX2_OK)
        {
            T res = MatrixMath::sum<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2>>(threadTrait, rows, cols, a.data());
            TestUtility::compareValues(res, expected, "Threaded SIMD AVX2", _tolerance);
        }
        #endif

        #if defined(STALKER_SIMD_AVX512_OK)
        {
            T res = MatrixMath::sum<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512>>(threadTrait, rows, cols, a.data());
            TestUtility::compareValues(res, expected, "Threaded SIMD AVX512", _tolerance);
        }
        #endif
        #endif
    }

    template<typename T>
    void _testScale() {
        printSubtitle("Matrix Scale " + TestUtility::getTypeString<T>(), T_Color::BARBIE_PINK);
        
        size_t rows = static_cast<size_t>(std::sqrt(_size));
        size_t cols = rows;
        size_t total = rows * cols;

        auto a = createAlignedVector<T>(total);
        Random::uniform<T>(total, a.data(), 0, 100);
        T scalar = static_cast<T>(2.5);
        
        auto expected = createAlignedVector<T>(total);
        for(size_t i=0; i<total; ++i) expected[i] = a[i] * scalar;

        // Classic
        {
            auto res = createAlignedVector<T>(total);
            MemoryOperations::copy<T>(total, res.data(), a.data());
            MatrixMath::scale<T, ExecutionTraitClassic<>>(rows, cols, res.data(), scalar);
            TestUtility::compareVectors(res.data(), expected.data(), total, "Classic", _tolerance);
        }

        // Meta
        {
            auto res = createAlignedVector<T>(total);
            MemoryOperations::copy<T>(total, res.data(), a.data());
            MatrixMath::scale<T, ExecutionTraitUnrolled<>>(rows, cols, res.data(), scalar);
            TestUtility::compareVectors(res.data(), expected.data(), total, "Meta", _tolerance);
        }

        #if defined(STALKER_SIMD_AVX2_OK)
        {
            auto res = createAlignedVector<T>(total);
            MemoryOperations::copy<T>(total, res.data(), a.data());
            MatrixMath::scale<T, ExecutionTraitSIMD<T_SIMD::AVX2>>(rows, cols, res.data(), scalar);
            TestUtility::compareVectors(res.data(), expected.data(), total, "SIMD AVX2", _tolerance);
        }
        #endif

        #if defined(STALKER_SIMD_AVX512_OK)
        {
            auto res = createAlignedVector<T>(total);
            MemoryOperations::copy<T>(total, res.data(), a.data());
            MatrixMath::scale<T, ExecutionTraitSIMD<T_SIMD::AVX512>>(rows, cols, res.data(), scalar);
            TestUtility::compareVectors(res.data(), expected.data(), total, "SIMD AVX512", _tolerance);
        }
        #endif

        #if defined(STALKER_THREADING_ENABLE) && STALKER_THREADING_ENABLE != 0
        ThreadTraitSTDThread threadTrait;
        
        {
            auto res = createAlignedVector<T>(total);
            MemoryOperations::copy<T>(total, res.data(), a.data());
            MatrixMath::scale<T, ThreadTraitSTDThread, ExecutionTraitClassic<>>(threadTrait, rows, cols, res.data(), scalar);
            TestUtility::compareVectors(res.data(), expected.data(), total, "Threaded Classic", _tolerance);
        }

        {
            auto res = createAlignedVector<T>(total);
            MemoryOperations::copy<T>(total, res.data(), a.data());
            MatrixMath::scale<T, ThreadTraitSTDThread, ExecutionTraitUnrolled<>>(threadTrait, rows, cols, res.data(), scalar);
            TestUtility::compareVectors(res.data(), expected.data(), total, "Threaded Meta", _tolerance);
        }

        #if defined(STALKER_SIMD_AVX2_OK)
        {
            auto res = createAlignedVector<T>(total);
            MemoryOperations::copy<T>(total, res.data(), a.data());
            MatrixMath::scale<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2>>(threadTrait, rows, cols, res.data(), scalar);
            TestUtility::compareVectors(res.data(), expected.data(), total, "Threaded SIMD AVX2", _tolerance);
        }
        #endif

        #if defined(STALKER_SIMD_AVX512_OK)
        {
            auto res = createAlignedVector<T>(total);
            MemoryOperations::copy<T>(total, res.data(), a.data());
            MatrixMath::scale<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512>>(threadTrait, rows, cols, res.data(), scalar);
            TestUtility::compareVectors(res.data(), expected.data(), total, "Threaded SIMD AVX512", _tolerance);
        }
        #endif
        #endif
    }

    template<typename T>
    void _testAddConstant() {
        printSubtitle("Matrix Add Constant " + TestUtility::getTypeString<T>(), T_Color::BARBIE_PINK);
        
        size_t rows = static_cast<size_t>(std::sqrt(_size));
        size_t cols = rows;
        size_t total = rows * cols;

        auto a = createAlignedVector<T>(total);
        Random::uniform<T>(total, a.data(), 0, 100);
        T constant = static_cast<T>(5);
        
        auto expected = createAlignedVector<T>(total);
        for(size_t i=0; i<total; ++i) expected[i] = a[i] + constant;

        // Classic
        {
            auto res = createAlignedVector<T>(total);
            MemoryOperations::copy<T>(total, res.data(), a.data());
            MatrixMath::addConstant<T, ExecutionTraitClassic<>>(rows, cols, res.data(), constant);
            TestUtility::compareVectors(res.data(), expected.data(), total, "Classic", _tolerance);
        }

        // Meta
        {
            auto res = createAlignedVector<T>(total);
            MemoryOperations::copy<T>(total, res.data(), a.data());
            MatrixMath::addConstant<T, ExecutionTraitUnrolled<>>(rows, cols, res.data(), constant);
            TestUtility::compareVectors(res.data(), expected.data(), total, "Meta", _tolerance);
        }

        #if defined(STALKER_SIMD_AVX2_OK)
        {
            auto res = createAlignedVector<T>(total);
            MemoryOperations::copy<T>(total, res.data(), a.data());
            MatrixMath::addConstant<T, ExecutionTraitSIMD<T_SIMD::AVX2>>(rows, cols, res.data(), constant);
            TestUtility::compareVectors(res.data(), expected.data(), total, "SIMD AVX2", _tolerance);
        }
        #endif

        #if defined(STALKER_SIMD_AVX512_OK)
        {
            auto res = createAlignedVector<T>(total);
            MemoryOperations::copy<T>(total, res.data(), a.data());
            MatrixMath::addConstant<T, ExecutionTraitSIMD<T_SIMD::AVX512>>(rows, cols, res.data(), constant);
            TestUtility::compareVectors(res.data(), expected.data(), total, "SIMD AVX512", _tolerance);
        }
        #endif

        #if defined(STALKER_THREADING_ENABLE) && STALKER_THREADING_ENABLE != 0
        ThreadTraitSTDThread threadTrait;
        
        {
            auto res = createAlignedVector<T>(total);
            MemoryOperations::copy<T>(total, res.data(), a.data());
            MatrixMath::addConstant<T, ThreadTraitSTDThread, ExecutionTraitClassic<>>(threadTrait, rows, cols, res.data(), constant);
            TestUtility::compareVectors(res.data(), expected.data(), total, "Threaded Classic", _tolerance);
        }

        {
            auto res = createAlignedVector<T>(total);
            MemoryOperations::copy<T>(total, res.data(), a.data());
            MatrixMath::addConstant<T, ThreadTraitSTDThread, ExecutionTraitUnrolled<>>(threadTrait, rows, cols, res.data(), constant);
            TestUtility::compareVectors(res.data(), expected.data(), total, "Threaded Meta", _tolerance);
        }

        #if defined(STALKER_SIMD_AVX2_OK)
        {
            auto res = createAlignedVector<T>(total);
            MemoryOperations::copy<T>(total, res.data(), a.data());
            MatrixMath::addConstant<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2>>(threadTrait, rows, cols, res.data(), constant);
            TestUtility::compareVectors(res.data(), expected.data(), total, "Threaded SIMD AVX2", _tolerance);
        }
        #endif

        #if defined(STALKER_SIMD_AVX512_OK)
        {
            auto res = createAlignedVector<T>(total);
            MemoryOperations::copy<T>(total, res.data(), a.data());
            MatrixMath::addConstant<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512>>(threadTrait, rows, cols, res.data(), constant);
            TestUtility::compareVectors(res.data(), expected.data(), total, "Threaded SIMD AVX512", _tolerance);
        }
        #endif
        #endif
    }

    template<typename T>
    void _testMatrixVectorMultiply() {
        printSubtitle("Matrix-Vector Multiply " + TestUtility::getTypeString<T>(), T_Color::BARBIE_PINK);
        double tolerance = _tolerance;
        if constexpr (std::is_same_v<T, float>) tolerance = 1e-3;

        size_t rows = static_cast<size_t>(std::sqrt(_size));
        size_t cols = rows;

        auto A = createAlignedVector<T>(rows * cols);
        auto x = createAlignedVector<T>(rows);
        Random::uniform<T>(rows * cols, A.data(), 0, 2);
        Random::uniform<T>(rows, x.data(), 0, 2);

        // Row Major
        {
            auto stride = cols; // No padding
            auto expected = createAlignedVector<T>(rows);
            for(size_t i=0; i<rows; ++i) {
                T sum = 0;
                for(size_t j=0; j<cols; ++j) sum += A[i*stride + j] * x[j];
                expected[i] = sum;
            }
            
            // Classic
            {
                auto res = createAlignedVector<T>(rows);
                MatrixMath::matrixVectorMultiply<T, true, ExecutionTraitClassic<>>(rows, cols, A.data(), x.data(), res.data(), stride);
                TestUtility::compareVectors(res.data(), expected.data(), rows, "RowMajor Classic", tolerance);
            }
            // Meta
            {
                auto res = createAlignedVector<T>(rows);
                MatrixMath::matrixVectorMultiply<T, true, ExecutionTraitUnrolled<>>(rows, cols, A.data(), x.data(), res.data(), stride);
                TestUtility::compareVectors(res.data(), expected.data(), rows, "RowMajor Meta", tolerance);
            }
            #if defined(STALKER_SIMD_AVX2_OK)
            {
                auto res = createAlignedVector<T>(rows);
                MatrixMath::matrixVectorMultiply<T, true, ExecutionTraitSIMD<T_SIMD::AVX2>>(rows, cols, A.data(), x.data(), res.data(), stride);
                TestUtility::compareVectors(res.data(), expected.data(), rows, "RowMajor SIMD AVX2", tolerance);
            }
            #endif
            #if defined(STALKER_SIMD_AVX512_OK)
            {
                auto res = createAlignedVector<T>(rows);
                MatrixMath::matrixVectorMultiply<T, true, ExecutionTraitSIMD<T_SIMD::AVX512>>(rows, cols, A.data(), x.data(), res.data(), stride);
                TestUtility::compareVectors(res.data(), expected.data(), rows, "RowMajor SIMD AVX512", tolerance);
            }
            #endif

            #if defined(STALKER_THREADING_ENABLE) && STALKER_THREADING_ENABLE != 0
            ThreadTraitSTDThread threadTrait;
            // Threaded Classic
            {
                auto resThreaded = createAlignedVector<T>(rows);
                MatrixMath::matrixVectorMultiply<T, true, ThreadTraitSTDThread, ExecutionTraitClassic<>>(threadTrait, rows, cols, A.data(), x.data(), resThreaded.data(), stride);
                TestUtility::compareVectors(resThreaded.data(), expected.data(), rows, "RowMajor Threaded Classic", tolerance);
            }
            // Threaded Meta
            {
                auto resThreaded = createAlignedVector<T>(rows);
                MatrixMath::matrixVectorMultiply<T, true, ThreadTraitSTDThread, ExecutionTraitUnrolled<>>(threadTrait, rows, cols, A.data(), x.data(), resThreaded.data(), stride);
                TestUtility::compareVectors(resThreaded.data(), expected.data(), rows, "RowMajor Threaded Meta", tolerance);
            }
            #if defined(STALKER_SIMD_AVX2_OK)
            {
                auto resThreaded = createAlignedVector<T>(rows);
                MatrixMath::matrixVectorMultiply<T, true, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2>>(threadTrait, rows, cols, A.data(), x.data(), resThreaded.data(), stride);
                TestUtility::compareVectors(resThreaded.data(), expected.data(), rows, "RowMajor Threaded SIMD AVX2", tolerance);
            }
            #endif
            #if defined(STALKER_SIMD_AVX512_OK)
            {
                auto resThreaded = createAlignedVector<T>(rows);
                MatrixMath::matrixVectorMultiply<T, true, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512>>(threadTrait, rows, cols, A.data(), x.data(), resThreaded.data(), stride);
                TestUtility::compareVectors(resThreaded.data(), expected.data(), rows, "RowMajor Threaded SIMD AVX512", tolerance);
            }
            #endif
            #endif
        }

        // Col Major
        {
            size_t stride = rows; // No padding
            auto expected = createAlignedVector<T>(rows);
            for(size_t i=0; i<rows; ++i) expected[i] = 0;
            for(size_t j=0; j<cols; ++j) {
                for(size_t i=0; i<rows; ++i) expected[i] += A[j*stride + i] * x[j];
            }

            // Classic
            {
                auto res = createAlignedVector<T>(rows);
                MatrixMath::matrixVectorMultiply<T, false, ExecutionTraitClassic<>>(rows, cols, A.data(), x.data(), res.data(), stride);
                TestUtility::compareVectors(res.data(), expected.data(), rows, "ColMajor Classic", tolerance);
            }
            // Meta
            {
                auto res = createAlignedVector<T>(rows);
                MatrixMath::matrixVectorMultiply<T, false, ExecutionTraitUnrolled<>>(rows, cols, A.data(), x.data(), res.data(), stride);
                TestUtility::compareVectors(res.data(), expected.data(), rows, "ColMajor Meta", tolerance);
            }
            #if defined(STALKER_SIMD_AVX2_OK)
            {
                auto res = createAlignedVector<T>(rows);
                MatrixMath::matrixVectorMultiply<T, false, ExecutionTraitSIMD<T_SIMD::AVX2>>(rows, cols, A.data(), x.data(), res.data(), stride);
                TestUtility::compareVectors(res.data(), expected.data(), rows, "ColMajor SIMD AVX2", tolerance);
            }
            #endif
            #if defined(STALKER_SIMD_AVX512_OK)
            {
                auto res = createAlignedVector<T>(rows);
                MatrixMath::matrixVectorMultiply<T, false, ExecutionTraitSIMD<T_SIMD::AVX512>>(rows, cols, A.data(), x.data(), res.data(), stride);
                TestUtility::compareVectors(res.data(), expected.data(), rows, "ColMajor SIMD AVX512", tolerance);
            }
            #endif

            #if defined(STALKER_THREADING_ENABLE) && STALKER_THREADING_ENABLE != 0
            ThreadTraitSTDThread threadTrait;
            // Threaded Classic
            {
                auto resThreaded = createAlignedVector<T>(rows);
                MatrixMath::matrixVectorMultiply<T, false, ThreadTraitSTDThread, ExecutionTraitClassic<>>(threadTrait, rows, cols, A.data(), x.data(), resThreaded.data(), stride);
                TestUtility::compareVectors(resThreaded.data(), expected.data(), rows, "ColMajor Threaded Classic", tolerance);
            }
            // Threaded Meta
            {
                auto resThreaded = createAlignedVector<T>(rows);
                MatrixMath::matrixVectorMultiply<T, false, ThreadTraitSTDThread, ExecutionTraitUnrolled<>>(threadTrait, rows, cols, A.data(), x.data(), resThreaded.data(), stride);
                TestUtility::compareVectors(resThreaded.data(), expected.data(), rows, "ColMajor Threaded Meta", tolerance);
            }
            #if defined(STALKER_SIMD_AVX2_OK)
            {
                auto resThreaded = createAlignedVector<T>(rows);
                MatrixMath::matrixVectorMultiply<T, false, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2>>(threadTrait, rows, cols, A.data(), x.data(), resThreaded.data(), stride);
                TestUtility::compareVectors(resThreaded.data(), expected.data(), rows, "ColMajor Threaded SIMD AVX2", tolerance);
            }
            #endif
            #if defined(STALKER_SIMD_AVX512_OK)
            {
                auto resThreaded = createAlignedVector<T>(rows);
                MatrixMath::matrixVectorMultiply<T, false, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512>>(threadTrait, rows, cols, A.data(), x.data(), resThreaded.data(), stride);
                TestUtility::compareVectors(resThreaded.data(), expected.data(), rows, "ColMajor Threaded SIMD AVX512", tolerance);
            }
            #endif
            #endif
        }
    }

    template<typename T>
    void _testVectorMatrixMultiply() {
        printSubtitle("Vector-Matrix Multiply " + TestUtility::getTypeString<T>(), T_Color::BARBIE_PINK);
        double tolerance = _tolerance;
        if constexpr (std::is_same_v<T, float>) tolerance = 1e-3;

        size_t rows = static_cast<size_t>(std::sqrt(_size));
        size_t cols = rows;
        
        // Calculate aligned stride
        constexpr size_t alignment = 64; 
        size_t elementSize = sizeof(T);
        size_t elementsPerBlock = alignment / elementSize;
        size_t stride = (cols + elementsPerBlock - 1) / elementsPerBlock * elementsPerBlock;

        auto A = createAlignedVector<T>(rows * stride);
        auto x = createAlignedVector<T>(rows);
        Random::uniform<T>(rows * stride, A.data(), 0, 10);
        Random::uniform<T>(rows, x.data(), 0, 10);

        // Row Major: y = x * A. y is 1xCols. x is 1xRows. A is RowsxCols.
        // y_j = sum_i x_i * A_ij
        {
            auto expected = createAlignedVector<T>(cols);
            for(size_t j=0; j<cols; ++j) {
                T sum = 0;
                for(size_t i=0; i<rows; ++i) sum += x[i] * A[i*stride + j];
                expected[j] = sum;
            }
            
            // Classic
            {
                auto res = createAlignedVector<T>(cols);
                MatrixMath::vectorMatrixMultiply<T, true, ExecutionTraitClassic<>>(rows, cols, x.data(), A.data(), res.data(), stride);
                TestUtility::compareVectors(res.data(), expected.data(), cols, "RowMajor Classic", tolerance);
            }
            // Meta
            {
                auto res = createAlignedVector<T>(cols);
                MatrixMath::vectorMatrixMultiply<T, true, ExecutionTraitUnrolled<>>(rows, cols, x.data(), A.data(), res.data(), stride);
                TestUtility::compareVectors(res.data(), expected.data(), cols, "RowMajor Meta", tolerance);
            }
            #if defined(STALKER_SIMD_AVX2_OK)
            {
                auto res = createAlignedVector<T>(cols);
                MatrixMath::vectorMatrixMultiply<T, true, ExecutionTraitSIMD<T_SIMD::AVX2>>(rows, cols, x.data(), A.data(), res.data(), stride);
                TestUtility::compareVectors(res.data(), expected.data(), cols, "RowMajor SIMD AVX2", tolerance);
            }
            #endif
            #if defined(STALKER_SIMD_AVX512_OK)
            {
                auto res = createAlignedVector<T>(cols);
                MatrixMath::vectorMatrixMultiply<T, true, ExecutionTraitSIMD<T_SIMD::AVX512>>(rows, cols, x.data(), A.data(), res.data(), stride);
                TestUtility::compareVectors(res.data(), expected.data(), cols, "RowMajor SIMD AVX512", tolerance);
            }
            #endif

            #if defined(STALKER_THREADING_ENABLE) && STALKER_THREADING_ENABLE != 0
            ThreadTraitSTDThread threadTrait;
            // Threaded Classic
            {
                auto resThreaded = createAlignedVector<T>(cols);
                MatrixMath::vectorMatrixMultiply<T, true, ThreadTraitSTDThread, ExecutionTraitClassic<>>(threadTrait, rows, cols, x.data(), A.data(), resThreaded.data(), stride);
                TestUtility::compareVectors(resThreaded.data(), expected.data(), cols, "RowMajor Threaded Classic", tolerance);
            }
            // Threaded Meta
            {
                auto resThreaded = createAlignedVector<T>(cols);
                MatrixMath::vectorMatrixMultiply<T, true, ThreadTraitSTDThread, ExecutionTraitUnrolled<>>(threadTrait, rows, cols, x.data(), A.data(), resThreaded.data(), stride);
                TestUtility::compareVectors(resThreaded.data(), expected.data(), cols, "RowMajor Threaded Meta", tolerance);
            }
            #if defined(STALKER_SIMD_AVX2_OK)
            {
                auto resThreaded = createAlignedVector<T>(cols);
                MatrixMath::vectorMatrixMultiply<T, true, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2>>(threadTrait, rows, cols, x.data(), A.data(), resThreaded.data(), stride);
                TestUtility::compareVectors(resThreaded.data(), expected.data(), cols, "RowMajor Threaded SIMD AVX2", tolerance);
            }
            #endif
            #if defined(STALKER_SIMD_AVX512_OK)
            {
                auto resThreaded = createAlignedVector<T>(cols);
                MatrixMath::vectorMatrixMultiply<T, true, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512>>(threadTrait, rows, cols, x.data(), A.data(), resThreaded.data(), stride);
                TestUtility::compareVectors(resThreaded.data(), expected.data(), cols, "RowMajor Threaded SIMD AVX512", tolerance);
            }
            #endif
            #endif
        }

        // Col Major: y = x * A.
        // A is stored col-major. A_ij is at j*rows + i.
        // y_j = sum_i x_i * A_ij
        {
            auto expected = createAlignedVector<T>(cols);
            for(size_t j=0; j<cols; ++j) {
                T sum = 0;
                for(size_t i=0; i<rows; ++i) sum += x[i] * A[j*stride + i];
                expected[j] = sum;
            }

            // Classic
            {
                auto res = createAlignedVector<T>(cols);
                MatrixMath::vectorMatrixMultiply<T, false, ExecutionTraitClassic<>>(rows, cols, x.data(), A.data(), res.data(), stride);
                TestUtility::compareVectors(res.data(), expected.data(), cols, "ColMajor Classic", tolerance);
            }
            // Meta
            {
                auto res = createAlignedVector<T>(cols);
                MatrixMath::vectorMatrixMultiply<T, false, ExecutionTraitUnrolled<>>(rows, cols, x.data(), A.data(), res.data(), stride);
                TestUtility::compareVectors(res.data(), expected.data(), cols, "ColMajor Meta", tolerance);
            }
            #if defined(STALKER_SIMD_AVX2_OK)
            {
                auto res = createAlignedVector<T>(cols);
                MatrixMath::vectorMatrixMultiply<T, false, ExecutionTraitSIMD<T_SIMD::AVX2>>(rows, cols, x.data(), A.data(), res.data(), stride);
                TestUtility::compareVectors(res.data(), expected.data(), cols, "ColMajor SIMD AVX2", tolerance);
            }
            #endif
            #if defined(STALKER_SIMD_AVX512_OK)
            {
                auto res = createAlignedVector<T>(cols);
                MatrixMath::vectorMatrixMultiply<T, false, ExecutionTraitSIMD<T_SIMD::AVX512>>(rows, cols, x.data(), A.data(), res.data(), stride);
                TestUtility::compareVectors(res.data(), expected.data(), cols, "ColMajor SIMD AVX512", tolerance);
            }
            #endif

            #if defined(STALKER_THREADING_ENABLE) && STALKER_THREADING_ENABLE != 0
            ThreadTraitSTDThread threadTrait;
            // Threaded Classic
            {
                auto resThreaded = createAlignedVector<T>(cols);
                MatrixMath::vectorMatrixMultiply<T, false, ThreadTraitSTDThread, ExecutionTraitClassic<>>(threadTrait, rows, cols, x.data(), A.data(), resThreaded.data(), stride);
                TestUtility::compareVectors(resThreaded.data(), expected.data(), cols, "ColMajor Threaded Classic", tolerance);
            }
            // Threaded Meta
            {
                auto resThreaded = createAlignedVector<T>(cols);
                MatrixMath::vectorMatrixMultiply<T, false, ThreadTraitSTDThread, ExecutionTraitUnrolled<>>(threadTrait, rows, cols, x.data(), A.data(), resThreaded.data(), stride);
                TestUtility::compareVectors(resThreaded.data(), expected.data(), cols, "ColMajor Threaded Meta", tolerance);
            }
            #if defined(STALKER_SIMD_AVX2_OK)
            {
                auto resThreaded = createAlignedVector<T>(cols);
                MatrixMath::vectorMatrixMultiply<T, false, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2>>(threadTrait, rows, cols, x.data(), A.data(), resThreaded.data(), stride);
                TestUtility::compareVectors(resThreaded.data(), expected.data(), cols, "ColMajor Threaded SIMD AVX2", tolerance);
            }
            #endif
            #if defined(STALKER_SIMD_AVX512_OK)
            {
                auto resThreaded = createAlignedVector<T>(cols);
                MatrixMath::vectorMatrixMultiply<T, false, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512>>(threadTrait, rows, cols, x.data(), A.data(), resThreaded.data(), stride);
                TestUtility::compareVectors(resThreaded.data(), expected.data(), cols, "ColMajor Threaded SIMD AVX512", tolerance);
            }
            #endif
            #endif
        }
    }

};


} // namespace STLKR_Tests