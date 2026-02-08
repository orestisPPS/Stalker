#pragma once
#include <cmath>
#include <cstring>
#include "STLKR_TestBase.h"
#include <Stalker/Mathematics/Matrix/MatrixMath.h>

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
        
        _testAdd<double>();
        _testAdd<float>();
        _testAdd<int>();
        
        _testSubtract<double>();
        _testSubtract<float>();
        _testSubtract<int>();

        _testMultiply<double>();
        _testMultiply<float>();
        _testMultiply<int>();

        _testSum<double>();
        _testSum<float>();
        _testSum<int>();

        _testScale<double>();
        _testScale<float>();
        _testScale<int>();

        _testAddConstant<double>();
        _testAddConstant<float>();
        _testAddConstant<int>();

        _testMatrixVectorMultiply<double>();
        _testMatrixVectorMultiply<float>();
        _testMatrixVectorMultiply<int>();

        _testVectorMatrixMultiply<double>();
        _testVectorMatrixMultiply<float>();
        _testVectorMatrixMultiply<int>();
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
            MatrixMath::add<T, ExecutionTraitScalar<>>(rows, cols, a.data(), b.data(), res.data());
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
            MatrixMath::add<T, ThreadTraitSTDThread, ExecutionTraitScalar<>>(threadTrait, rows, cols, a.data(), b.data(), res.data());
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
            MatrixMath::subtract<T, true, ExecutionTraitScalar<>>(rows, cols, a.data(), b.data(), res.data());
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
            MatrixMath::subtract<T, true, ThreadTraitSTDThread, ExecutionTraitScalar<>>(threadTrait, rows, cols, a.data(), b.data(), res.data());
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
            MatrixMath::multiply<T, ExecutionTraitScalar<>>(rows, cols, a.data(), b.data(), res.data());
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
            MatrixMath::multiply<T, ThreadTraitSTDThread, ExecutionTraitScalar<>>(threadTrait, rows, cols, a.data(), b.data(), res.data());
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
            T res = MatrixMath::sum<T, ExecutionTraitScalar<>>(rows, cols, a.data());
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
            T res = MatrixMath::sum<T, ThreadTraitSTDThread, ExecutionTraitScalar<>>(threadTrait, rows, cols, a.data());
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
            MatrixMath::scale<T, ExecutionTraitScalar<>>(rows, cols, res.data(), scalar);
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
            MatrixMath::scale<T, ThreadTraitSTDThread, ExecutionTraitScalar<>>(threadTrait, rows, cols, res.data(), scalar);
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
            std::copy_n(a.data(), total, res.data());
            MatrixMath::addConstant<T, ExecutionTraitScalar<>>(rows, cols, res.data(), constant);
            TestUtility::compareVectors(res.data(), expected.data(), total, "Classic", _tolerance);
        }

        // Meta
        {
            auto res = createAlignedVector<T>(total);
            std::copy_n(a.data(), total, res.data());
            MatrixMath::addConstant<T, ExecutionTraitUnrolled<>>(rows, cols, res.data(), constant);
            TestUtility::compareVectors(res.data(), expected.data(), total, "Meta", _tolerance);
        }

        #if defined(STALKER_SIMD_AVX2_OK)
        {
            auto res = createAlignedVector<T>(total);
            std::copy_n(a.data(), total, res.data());
            MatrixMath::addConstant<T, ExecutionTraitSIMD<T_SIMD::AVX2>>(rows, cols, res.data(), constant);
            TestUtility::compareVectors(res.data(), expected.data(), total, "SIMD AVX2", _tolerance);
        }
        #endif

        #if defined(STALKER_SIMD_AVX512_OK)
        {
            auto res = createAlignedVector<T>(total);
            std::copy_n(a.data(), total, res.data());
            MatrixMath::addConstant<T, ExecutionTraitSIMD<T_SIMD::AVX512>>(rows, cols, res.data(), constant);
            TestUtility::compareVectors(res.data(), expected.data(), total, "SIMD AVX512", _tolerance);
        }
        #endif

        #if defined(STALKER_THREADING_ENABLE) && STALKER_THREADING_ENABLE != 0
        ThreadTraitSTDThread threadTrait;
        
        {
            auto res = createAlignedVector<T>(total);
            std::copy_n(a.data(), total, res.data());
            MatrixMath::addConstant<T, ThreadTraitSTDThread, ExecutionTraitScalar<>>(threadTrait, rows, cols, res.data(), constant);
            TestUtility::compareVectors(res.data(), expected.data(), total, "Threaded Classic", _tolerance);
        }

        {
            auto res = createAlignedVector<T>(total);
            std::copy_n(a.data(), total, res.data());
            MatrixMath::addConstant<T, ThreadTraitSTDThread, ExecutionTraitUnrolled<>>(threadTrait, rows, cols, res.data(), constant);
            TestUtility::compareVectors(res.data(), expected.data(), total, "Threaded Meta", _tolerance);
        }

        #if defined(STALKER_SIMD_AVX2_OK)
        {
            auto res = createAlignedVector<T>(total);
            std::copy_n(a.data(), total, res.data());
            MatrixMath::addConstant<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2>>(threadTrait, rows, cols, res.data(), constant);
            TestUtility::compareVectors(res.data(), expected.data(), total, "Threaded SIMD AVX2", _tolerance);
        }
        #endif

        #if defined(STALKER_SIMD_AVX512_OK)
        {
            auto res = createAlignedVector<T>(total);
            std::copy_n(a.data(), total, res.data());
            MatrixMath::addConstant<T, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512>>(threadTrait, rows, cols, res.data(), constant);
            TestUtility::compareVectors(res.data(), expected.data(), total, "Threaded SIMD AVX512", _tolerance);
        }
        #endif
        #endif
    }
    template<typename T>
    void _testMatrixVectorMultiply() {
        printSubtitle("Matrix-Vector Multiply " + TestUtility::getTypeString<T>() + " Row Major", T_Color::BARBIE_PINK);
        double tolerance = _tolerance;
        if constexpr (std::is_same_v<T, float>) tolerance = 1e-1;

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
                MatrixMath::matrixVectorMultiply<T, true, ExecutionTraitScalar<>>(rows, cols, A.data(), x.data(), res.data(), stride);
                TestUtility::compareVectors(res.data(), expected.data(), rows, "Classic", tolerance);
            }
            // Meta
            {
                auto res = createAlignedVector<T>(rows);
                MatrixMath::matrixVectorMultiply<T, true, ExecutionTraitUnrolled<>>(rows, cols, A.data(), x.data(), res.data(), stride);
                TestUtility::compareVectors(res.data(), expected.data(), rows, "Meta", tolerance);
            }

            #if defined(STALKER_SIMD_AVX2_OK) || defined(STALKER_SIMD_AVX512_OK)
            
            size_t padding = getPadding<T>(cols);
            auto APadded = createAlignedVector<T>(rows * (cols + padding));

            for(size_t i=0; i<rows; ++i) {
                for(size_t j=0; j<cols; ++j) {
                    APadded[i * (cols + padding) + j] = A[i * cols + j];
                }
            }

            #endif


            #if defined(STALKER_SIMD_AVX2_OK)
            {
                auto res = createAlignedVector<T>(rows);
                MatrixMath::matrixVectorMultiply<T, true, ExecutionTraitSIMD<T_SIMD::AVX2, false>>(rows, cols, A.data(), x.data(), res.data(), stride);
                TestUtility::compareVectors(res.data(), expected.data(), rows, "SIMD AVX2 Unaligned Load", tolerance);

                auto res2 = createAlignedVector<T>(rows);
                MatrixMath::matrixVectorMultiply<T, true, ExecutionTraitSIMD<T_SIMD::AVX2, true>>(rows, cols, APadded.data(), x.data(), res2.data(), stride + padding);
                TestUtility::compareVectors(res2.data(), expected.data(), rows, "SIMD AVX2 Aligned Load Padding", tolerance);
            }
            #endif
            #if defined(STALKER_SIMD_AVX512_OK)
            {
                auto res = createAlignedVector<T>(rows);
                MatrixMath::matrixVectorMultiply<T, true, ExecutionTraitSIMD<T_SIMD::AVX512, false>>(rows, cols, A.data(), x.data(), res.data(), stride);
                TestUtility::compareVectors(res.data(), expected.data(), rows, "SIMD AVX512 Unaligned Load", tolerance);

                auto res2 = createAlignedVector<T>(rows);
                MatrixMath::matrixVectorMultiply<T, true, ExecutionTraitSIMD<T_SIMD::AVX512, true>>(rows, cols, APadded.data(), x.data(), res2.data(), stride + padding);
                TestUtility::compareVectors(res2.data(), expected.data(), rows, "SIMD AVX512 Aligned Load Padding", tolerance);
            }
            #endif

            #if defined(STALKER_THREADING_ENABLE) && STALKER_THREADING_ENABLE != 0
            ThreadTraitSTDThread threadTrait;
            // Threaded Classic
            {
                auto resThreaded = createAlignedVector<T>(rows);
                MatrixMath::matrixVectorMultiply<T, true, ThreadTraitSTDThread, ExecutionTraitScalar<>>(threadTrait, rows, cols, A.data(), x.data(), resThreaded.data(), stride);
                TestUtility::compareVectors(resThreaded.data(), expected.data(), rows, "Threaded Classic", tolerance);
            }
            // Threaded Meta
            {
                auto resThreaded = createAlignedVector<T>(rows);
                MatrixMath::matrixVectorMultiply<T, true, ThreadTraitSTDThread, ExecutionTraitUnrolled<>>(threadTrait, rows, cols, A.data(), x.data(), resThreaded.data(), stride);
                TestUtility::compareVectors(resThreaded.data(), expected.data(), rows, "Threaded Meta", tolerance);
            }
            #if defined(STALKER_SIMD_AVX2_OK)
            {
                auto resThreaded = createAlignedVector<T>(rows);
                MatrixMath::matrixVectorMultiply<T, true, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2>>(threadTrait, rows, cols, A.data(), x.data(), resThreaded.data(), stride);
                TestUtility::compareVectors(resThreaded.data(), expected.data(), rows, "Threaded SIMD AVX2", tolerance);

                auto resThreaded2 = createAlignedVector<T>(rows);
                MatrixMath::matrixVectorMultiply<T, true, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2, true>>(threadTrait, rows, cols, APadded.data(), x.data(), resThreaded2.data(), stride + padding);
                TestUtility::compareVectors(resThreaded2.data(), expected.data(), rows, "Threaded SIMD AVX2 Aligned Load Padding", tolerance);
            }
            #endif
            #if defined(STALKER_SIMD_AVX512_OK)
            {
                auto resThreaded = createAlignedVector<T>(rows);
                MatrixMath::matrixVectorMultiply<T, true, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512>>(threadTrait, rows, cols, A.data(), x.data(), resThreaded.data(), stride);
                TestUtility::compareVectors(resThreaded.data(), expected.data(), rows, "Threaded SIMD AVX512", tolerance);

                auto resThreaded2 = createAlignedVector<T>(rows);
                MatrixMath::matrixVectorMultiply<T, true, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512, true>>(threadTrait, rows, cols, APadded.data(), x.data(), resThreaded2.data(), stride + padding);
                TestUtility::compareVectors(resThreaded2.data(), expected.data(), rows, "Threaded SIMD AVX512 Aligned Load Padding", tolerance);
            }
            #endif
            #endif
        }

        // Col Major
        {
            printSubtitle("Matrix-Vector Multiply " + TestUtility::getTypeString<T>() + " Col Major", T_Color::BARBIE_PINK);

            size_t stride = rows; // No padding
            auto expected = createAlignedVector<T>(rows);
            for(size_t i=0; i<rows; ++i) expected[i] = 0;
            for(size_t j=0; j<cols; ++j) {
                for(size_t i=0; i<rows; ++i) expected[i] += A[j*stride + i] * x[j];
            }

            // Classic
            {
                auto res = createAlignedVector<T>(rows);
                MatrixMath::matrixVectorMultiply<T, false, ExecutionTraitScalar<>>(rows, cols, A.data(), x.data(), res.data(), stride);
                TestUtility::compareVectors(res.data(), expected.data(), rows, "Classic", tolerance);
            }
            // Meta
            {
                auto res = createAlignedVector<T>(rows);
                MatrixMath::matrixVectorMultiply<T, false, ExecutionTraitUnrolled<>>(rows, cols, A.data(), x.data(), res.data(), stride);
                TestUtility::compareVectors(res.data(), expected.data(), rows, "Meta", tolerance);
            }
            #if defined(STALKER_SIMD_AVX2_OK) || defined(STALKER_SIMD_AVX512_OK)
            
            size_t padding = getPadding<T>(rows);
            auto APadded = createAlignedVector<T>((rows + padding) * cols);

            for(size_t j=0; j<cols; ++j) {
                for(size_t i=0; i<rows; ++i) {
                    APadded[j * (rows + padding) + i] = A[j * rows + i];
                }
            }

            #endif

            #if defined(STALKER_SIMD_AVX2_OK)
            {
                auto res = createAlignedVector<T>(rows);
                MatrixMath::matrixVectorMultiply<T, false, ExecutionTraitSIMD<T_SIMD::AVX2, false>>(rows, cols, A.data(), x.data(), res.data(), stride);
                TestUtility::compareVectors(res.data(), expected.data(), rows, "SIMD AVX2 Unaligned Load", tolerance);

                auto res2 = createAlignedVector<T>(rows);
                MatrixMath::matrixVectorMultiply<T, false, ExecutionTraitSIMD<T_SIMD::AVX2, true>>(rows, cols, APadded.data(), x.data(), res2.data(), stride + padding);
                TestUtility::compareVectors(res2.data(), expected.data(), rows, "SIMD AVX2 Aligned Load Padding", tolerance);
            }
            #endif
            #if defined(STALKER_SIMD_AVX512_OK)
            {
                auto res = createAlignedVector<T>(rows);
                MatrixMath::matrixVectorMultiply<T, false, ExecutionTraitSIMD<T_SIMD::AVX512, false>>(rows, cols, A.data(), x.data(), res.data(), stride);
                TestUtility::compareVectors(res.data(), expected.data(), rows, "SIMD AVX512 Unaligned Load", tolerance);

                auto res2 = createAlignedVector<T>(rows);
                MatrixMath::matrixVectorMultiply<T, false, ExecutionTraitSIMD<T_SIMD::AVX512, true>>(rows, cols, APadded.data(), x.data(), res2.data(), stride + padding);
                TestUtility::compareVectors(res2.data(), expected.data(), rows, "SIMD AVX512 Aligned Load Padding", tolerance);
            }
            #endif

            #if defined(STALKER_THREADING_ENABLE) && STALKER_THREADING_ENABLE != 0
            ThreadTraitSTDThread threadTrait;
            // Threaded Classic
            {
                auto resThreaded = createAlignedVector<T>(rows);
                MatrixMath::matrixVectorMultiply<T, false, ThreadTraitSTDThread, ExecutionTraitScalar<>>(threadTrait, rows, cols, A.data(), x.data(), resThreaded.data(), stride);
                TestUtility::compareVectors(resThreaded.data(), expected.data(), rows, "Threaded Classic", tolerance);
            }
            // Threaded Meta
            {
                auto resThreaded = createAlignedVector<T>(rows);
                MatrixMath::matrixVectorMultiply<T, false, ThreadTraitSTDThread, ExecutionTraitUnrolled<>>(threadTrait, rows, cols, A.data(), x.data(), resThreaded.data(), stride);
                TestUtility::compareVectors(resThreaded.data(), expected.data(), rows, "Threaded Meta", tolerance);
            }
            #if defined(STALKER_SIMD_AVX2_OK)
            {
                auto resThreaded = createAlignedVector<T>(rows);
                MatrixMath::matrixVectorMultiply<T, false, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2, false>>(threadTrait, rows, cols, A.data(), x.data(), resThreaded.data(), stride);
                TestUtility::compareVectors(resThreaded.data(), expected.data(), rows, "Threaded SIMD AVX2 Unaligned Load", tolerance);

                auto resThreaded2 = createAlignedVector<T>(rows);
                MatrixMath::matrixVectorMultiply<T, false, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2, true>>(threadTrait, rows, cols, APadded.data(), x.data(), resThreaded2.data(), stride + padding);
                TestUtility::compareVectors(resThreaded2.data(), expected.data(), rows, "Threaded SIMD AVX2 Aligned Load Padding", tolerance);
            }
            #endif
            #if defined(STALKER_SIMD_AVX512_OK)
            {
                auto resThreaded = createAlignedVector<T>(rows);
                MatrixMath::matrixVectorMultiply<T, false, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512, false>>(threadTrait, rows, cols, A.data(), x.data(), resThreaded.data(), stride);
                TestUtility::compareVectors(resThreaded.data(), expected.data(), rows, "Threaded SIMD AVX512 Unaligned Load", tolerance);

                auto resThreaded2 = createAlignedVector<T>(rows);
                MatrixMath::matrixVectorMultiply<T, false, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512, true>>(threadTrait, rows, cols, APadded.data(), x.data(), resThreaded2.data(), stride + padding);
                TestUtility::compareVectors(resThreaded2.data(), expected.data(), rows, "Threaded SIMD AVX512 Aligned Load Padding", tolerance);
            }
            #endif
            #endif
        }
    }

    template<typename T>
    void _testVectorMatrixMultiply() {
        printSubtitle("Vector-Matrix Multiply " + TestUtility::getTypeString<T>() + " Row Major", T_Color::BARBIE_PINK);
        double tolerance = _tolerance;
        if constexpr (std::is_same_v<T, float>) tolerance = 1e-1;

        size_t rows = static_cast<size_t>(std::sqrt(_size));
        size_t cols = rows;
        
        auto A = createAlignedVector<T>(rows * cols);
        auto x = createAlignedVector<T>(rows);
        Random::uniform<T>(rows * cols, A.data(), 0, 10);
        Random::uniform<T>(rows, x.data(), 0, 10);

        // Row Major: y = x * A. y is 1xCols. x is 1xRows. A is RowsxCols.
        // y_j = sum_i x_i * A_ij
        {
            size_t stride = cols; // No padding
            auto expected = createAlignedVector<T>(cols);
            for(size_t j=0; j<cols; ++j) {
                T sum = 0;
                for(size_t i=0; i<rows; ++i) sum += x[i] * A[i*stride + j];
                expected[j] = sum;
            }
            
            // Classic
            {
                auto res = createAlignedVector<T>(cols);
                MatrixMath::vectorMatrixMultiply<T, true, ExecutionTraitScalar<>>(rows, cols, x.data(), A.data(), res.data(), stride);
                TestUtility::compareVectors(res.data(), expected.data(), cols, "Classic", tolerance);
            }
            // Meta
            {
                auto res = createAlignedVector<T>(cols);
                MatrixMath::vectorMatrixMultiply<T, true, ExecutionTraitUnrolled<>>(rows, cols, x.data(), A.data(), res.data(), stride);
                TestUtility::compareVectors(res.data(), expected.data(), cols, "Meta", tolerance);
            }

            #if defined(STALKER_SIMD_AVX2_OK) || defined(STALKER_SIMD_AVX512_OK)
            
            size_t padding = getPadding<T>(cols);
            auto APadded = createAlignedVector<T>(rows * (cols + padding));

            for(size_t i=0; i<rows; ++i) {
                for(size_t j=0; j<cols; ++j) {
                    APadded[i * (cols + padding) + j] = A[i * cols + j];
                }
            }

            #endif

            #if defined(STALKER_SIMD_AVX2_OK)
            {
                auto res = createAlignedVector<T>(cols);
                MatrixMath::vectorMatrixMultiply<T, true, ExecutionTraitSIMD<T_SIMD::AVX2, false>>(rows, cols, x.data(), A.data(), res.data(), stride);
                TestUtility::compareVectors(res.data(), expected.data(), cols, "SIMD AVX2 Unaligned Load", tolerance);

                auto res2 = createAlignedVector<T>(cols);
                MatrixMath::vectorMatrixMultiply<T, true, ExecutionTraitSIMD<T_SIMD::AVX2, true>>(rows, cols, x.data(), APadded.data(), res2.data(), stride + padding);
                TestUtility::compareVectors(res2.data(), expected.data(), cols, "SIMD AVX2 Aligned Load Padding", tolerance);
            }
            #endif
            #if defined(STALKER_SIMD_AVX512_OK)
            {
                auto res = createAlignedVector<T>(cols);
                MatrixMath::vectorMatrixMultiply<T, true, ExecutionTraitSIMD<T_SIMD::AVX512, false>>(rows, cols, x.data(), A.data(), res.data(), stride);
                TestUtility::compareVectors(res.data(), expected.data(), cols, "SIMD AVX512 Unaligned Load", tolerance);

                auto res2 = createAlignedVector<T>(cols);
                MatrixMath::vectorMatrixMultiply<T, true, ExecutionTraitSIMD<T_SIMD::AVX512, true>>(rows, cols, x.data(), APadded.data(), res2.data(), stride + padding);
                TestUtility::compareVectors(res2.data(), expected.data(), cols, "SIMD AVX512 Aligned Load Padding", tolerance);
            }
            #endif

            #if defined(STALKER_THREADING_ENABLE) && STALKER_THREADING_ENABLE != 0
            ThreadTraitSTDThread threadTrait;
            // Threaded Classic
            {
                auto resThreaded = createAlignedVector<T>(cols);
                MatrixMath::vectorMatrixMultiply<T, true, ThreadTraitSTDThread, ExecutionTraitScalar<>>(threadTrait, rows, cols, x.data(), A.data(), resThreaded.data(), stride);
                TestUtility::compareVectors(resThreaded.data(), expected.data(), cols, "Threaded Classic", tolerance);
            }
            // Threaded Meta
            {
                auto resThreaded = createAlignedVector<T>(cols);
                MatrixMath::vectorMatrixMultiply<T, true, ThreadTraitSTDThread, ExecutionTraitUnrolled<>>(threadTrait, rows, cols, x.data(), A.data(), resThreaded.data(), stride);
                TestUtility::compareVectors(resThreaded.data(), expected.data(), cols, "Threaded Meta", tolerance);
            }
            #if defined(STALKER_SIMD_AVX2_OK)
            {
                auto resThreaded = createAlignedVector<T>(cols);
                MatrixMath::vectorMatrixMultiply<T, true, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2, false>>(threadTrait, rows, cols, x.data(), A.data(), resThreaded.data(), stride);
                TestUtility::compareVectors(resThreaded.data(), expected.data(), cols, "Threaded SIMD AVX2 Unaligned Load", tolerance);

                auto resThreaded2 = createAlignedVector<T>(cols);
                MatrixMath::vectorMatrixMultiply<T, true, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2, true>>(threadTrait, rows, cols, x.data(), APadded.data(), resThreaded2.data(), stride + padding);
                TestUtility::compareVectors(resThreaded2.data(), expected.data(), cols, "Threaded SIMD AVX2 Aligned Load Padding", tolerance);
            }
            #endif
            #if defined(STALKER_SIMD_AVX512_OK)
            {
                auto resThreaded = createAlignedVector<T>(cols);
                MatrixMath::vectorMatrixMultiply<T, true, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512, false>>(threadTrait, rows, cols, x.data(), A.data(), resThreaded.data(), stride);
                TestUtility::compareVectors(resThreaded.data(), expected.data(), cols, "Threaded SIMD AVX512 Unaligned Load", tolerance);

                auto resThreaded2 = createAlignedVector<T>(cols);
                MatrixMath::vectorMatrixMultiply<T, true, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512, true>>(threadTrait, rows, cols, x.data(), APadded.data(), resThreaded2.data(), stride + padding);
                TestUtility::compareVectors(resThreaded2.data(), expected.data(), cols, "Threaded SIMD AVX512 Aligned Load Padding", tolerance);
            }
            #endif
            #endif
        }

        // Col Major: y = x * A.
        // A is stored col-major. A_ij is at j*rows + i.
        // y_j = sum_i x_i * A_ij
        {
            printSubtitle("Vector-Matrix Multiply " + TestUtility::getTypeString<T>() + " Col Major", T_Color::BARBIE_PINK);
            size_t stride = rows; // No padding
            auto expected = createAlignedVector<T>(cols);
            for(size_t j=0; j<cols; ++j) {
                T sum = 0;
                for(size_t i=0; i<rows; ++i) sum += x[i] * A[j*stride + i];
                expected[j] = sum;
            }

            // Classic
            {
                auto res = createAlignedVector<T>(cols);
                MatrixMath::vectorMatrixMultiply<T, false, ExecutionTraitScalar<>>(rows, cols, x.data(), A.data(), res.data(), stride);
                TestUtility::compareVectors(res.data(), expected.data(), cols, "Classic", tolerance);
            }
            // Meta
            {
                auto res = createAlignedVector<T>(cols);
                MatrixMath::vectorMatrixMultiply<T, false, ExecutionTraitUnrolled<>>(rows, cols, x.data(), A.data(), res.data(), stride);
                TestUtility::compareVectors(res.data(), expected.data(), cols, "Meta", tolerance);
            }

            #if defined(STALKER_SIMD_AVX2_OK) || defined(STALKER_SIMD_AVX512_OK)
            
            size_t padding = getPadding<T>(rows);
            auto APadded = createAlignedVector<T>((rows + padding) * cols);

            for(size_t j=0; j<cols; ++j) {
                for(size_t i=0; i<rows; ++i) {
                    APadded[j * (rows + padding) + i] = A[j * rows + i];
                }
            }

            #endif

            #if defined(STALKER_SIMD_AVX2_OK)
            {
                auto res = createAlignedVector<T>(cols);
                MatrixMath::vectorMatrixMultiply<T, false, ExecutionTraitSIMD<T_SIMD::AVX2, false>>(rows, cols, x.data(), A.data(), res.data(), stride);
                TestUtility::compareVectors(res.data(), expected.data(), cols, "SIMD AVX2 Unaligned Load", tolerance);

                auto res2 = createAlignedVector<T>(cols);
                MatrixMath::vectorMatrixMultiply<T, false, ExecutionTraitSIMD<T_SIMD::AVX2, true>>(rows, cols, x.data(), APadded.data(), res2.data(), stride + padding);
                TestUtility::compareVectors(res2.data(), expected.data(), cols, "SIMD AVX2 Aligned Load Padding", tolerance);
            }
            #endif
            #if defined(STALKER_SIMD_AVX512_OK)
            {
                auto res = createAlignedVector<T>(cols);
                MatrixMath::vectorMatrixMultiply<T, false, ExecutionTraitSIMD<T_SIMD::AVX512, false>>(rows, cols, x.data(), A.data(), res.data(), stride);
                TestUtility::compareVectors(res.data(), expected.data(), cols, "SIMD AVX512 Unaligned Load", tolerance);

                auto res2 = createAlignedVector<T>(cols);
                MatrixMath::vectorMatrixMultiply<T, false, ExecutionTraitSIMD<T_SIMD::AVX512, true>>(rows, cols, x.data(), APadded.data(), res2.data(), stride + padding);
                TestUtility::compareVectors(res2.data(), expected.data(), cols, "SIMD AVX512 Aligned Load Padding", tolerance);
            }
            #endif

            #if defined(STALKER_THREADING_ENABLE) && STALKER_THREADING_ENABLE != 0
            ThreadTraitSTDThread threadTrait;
            // Threaded Classic
            {
                auto resThreaded = createAlignedVector<T>(cols);
                MatrixMath::vectorMatrixMultiply<T, false, ThreadTraitSTDThread, ExecutionTraitScalar<>>(threadTrait, rows, cols, x.data(), A.data(), resThreaded.data(), stride);
                TestUtility::compareVectors(resThreaded.data(), expected.data(), cols, "Threaded Classic", tolerance);
            }
            // Threaded Meta
            {
                auto resThreaded = createAlignedVector<T>(cols);
                MatrixMath::vectorMatrixMultiply<T, false, ThreadTraitSTDThread, ExecutionTraitUnrolled<>>(threadTrait, rows, cols, x.data(), A.data(), resThreaded.data(), stride);
                TestUtility::compareVectors(resThreaded.data(), expected.data(), cols, "Threaded Meta", tolerance);
            }
            #if defined(STALKER_SIMD_AVX2_OK)
            {
                auto resThreaded = createAlignedVector<T>(cols);
                MatrixMath::vectorMatrixMultiply<T, false, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2, false>>(threadTrait, rows, cols, x.data(), A.data(), resThreaded.data(), stride);
                TestUtility::compareVectors(resThreaded.data(), expected.data(), cols, "Threaded SIMD AVX2 Unaligned Load", tolerance);

                auto resThreaded2 = createAlignedVector<T>(cols);
                MatrixMath::vectorMatrixMultiply<T, false, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX2, true>>(threadTrait, rows, cols, x.data(), APadded.data(), resThreaded2.data(), stride + padding);
                TestUtility::compareVectors(resThreaded2.data(), expected.data(), cols, "Threaded SIMD AVX2 Aligned Load Padding", tolerance);
            }
            #endif
            #if defined(STALKER_SIMD_AVX512_OK)
            {
                auto resThreaded = createAlignedVector<T>(cols);
                MatrixMath::vectorMatrixMultiply<T, false, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512, false>>(threadTrait, rows, cols, x.data(), A.data(), resThreaded.data(), stride);
                TestUtility::compareVectors(resThreaded.data(), expected.data(), cols, "Threaded SIMD AVX512 Unaligned Load", tolerance);

                auto resThreaded2 = createAlignedVector<T>(cols);
                MatrixMath::vectorMatrixMultiply<T, false, ThreadTraitSTDThread, ExecutionTraitSIMD<T_SIMD::AVX512, true>>(threadTrait, rows, cols, x.data(), APadded.data(), resThreaded2.data(), stride + padding);
                TestUtility::compareVectors(resThreaded2.data(), expected.data(), cols, "Threaded SIMD AVX512 Aligned Load Padding", tolerance);
            }
            #endif
            #endif
        }
    }

};


} // namespace STLKR_Tests