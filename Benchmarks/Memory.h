#pragma once

#include "Benchmark.h"
#include <Stalker/Memory/Allocators.h>
#include <Stalker/Memory/MemoryOperations.h>
#include <Stalker/Mathematics/Random.h>
#include <Stalker/Memory/Allocators.h>
#include <Stalker/Utility/Logs.h>
#include <cblas.h>
#include <Eigen/Dense>
#include "BenchConfig.hpp"   // from ${CMAKE_CURRENT_BINARY_DIR}

#include <array>
#include <vector>
#include <algorithm>

namespace Benchmarks {

    using namespace Stalker::Memory;
    using namespace Stalker::Utility;
    using namespace Stalker::Core::Config;

    class Memory {

    private:
    template<typename T>
    // void _testCopy(size_t size, Logs& logs) {
    void _testCopy(size_t size, Logs& logs) {

        std::string type = typeid(T).name();
        
        auto config = Benchmark::PAPIConfig();
        auto allocator = [&]() { return createAlignedVector<T>(size); };

        auto a = allocator();
        Stalker::Mathematics::Random::uniform<T>(size, a.data(), 0, 100);
        
        auto totalBytes = 2 * size * sizeof(T);

        constexpr size_t Unroll = 1;

        config.compareOver = {"memcpy " + type, "blas " + type};
        Benchmark::papiRun([&](auto& result) { MemoryOperations::copy<T, ExecutionTraitSIMD<T_SIMD::AVX2, Unroll>>(size, result.data(), a.data()); },
                    allocator, "stalker avx2 " + type, logs, config, totalBytes);

        Benchmark::papiRun([&](auto& result) { MemoryOperations::copy<T, ExecutionTraitSIMD<T_SIMD::AVX512, Unroll>>(size, result.data(), a.data()); },
                allocator, "stalker avx512 " + type, logs, config, totalBytes);

        config.compareOver = {};
        Benchmark::papiRun([&](auto& result) { MemoryOperations::copy<T, ExecutionTraitClassic<true>>(size, result.data(), a.data()); },
                allocator, "memcpy " + type, logs, config, totalBytes);

        Benchmark::papiRun([&](auto& result) { if constexpr (std::is_same_v<T, float>)
                                                    cblas_scopy(size, a.data(), 1, result.data(), 1);
                                            else if constexpr (std::is_same_v<T, double>)
                                                    cblas_dcopy(size, a.data(), 1, result.data(), 1);
                                            },
                allocator, "blas " + type, logs, config, totalBytes);
    }


    template<typename T>
    // void _testCopy(size_t size, Logs& logs) {
    void _testSetValue(size_t size, Logs& logs) {

        std::string type = typeid(T).name();
        
        auto config = Benchmark::PAPIConfig();
        auto allocator = [&]() { return createAlignedVector<T>(size); };
        
        auto totalBytes = 1 * size * sizeof(T);

        constexpr size_t Unroll = 1;

        auto value = static_cast<T>(3.14159265358979323846);

        config.compareOver = {"fill " + type, "blas " + type};
        Benchmark::papiRun([&](auto& result) { MemoryOperations::setValue<T, ExecutionTraitSIMD<T_SIMD::AVX2, Unroll>>(size, result.data(), value); },
                    allocator, "stalker avx2 " + type, logs, config, totalBytes);

        Benchmark::papiRun([&](auto& result) { MemoryOperations::setValue<T, ExecutionTraitSIMD<T_SIMD::AVX512, Unroll>>(size, result.data(), value); },
                allocator, "stalker avx512 " + type, logs, config, totalBytes);

        config.compareOver = {};
        Benchmark::papiRun([&](auto& result) { MemoryOperations::setValue<T, ExecutionTraitClassic<true>>(size, result.data(), value); },
                allocator, "fill " + type, logs, config, totalBytes);
        Benchmark::papiRun([&](auto& result) { if constexpr (std::is_same_v<T, float>)
                                                    cblas_sscal(static_cast<int>(size), value, result.data(), 1);
                                            else if constexpr (std::is_same_v<T, double>)
                                                    cblas_dscal(static_cast<int>(size), value, result.data(), 1);
                                            },
                allocator, "blas " + type, logs, config, totalBytes);
    }

    template<typename T>
    void _testSetZero(size_t size, Logs& logs) {

        std::string type = typeid(T).name();
        
        auto config = Benchmark::PAPIConfig();
        auto allocator = [&]() { return createAlignedVector<T>(size); };
        
        auto totalBytes = 1 * size * sizeof(T);

        constexpr size_t Unroll = 1;

        config.compareOver = {"memset " + type, "blas " + type};
        Benchmark::papiRun([&](auto& result) { MemoryOperations::setZero<T, ExecutionTraitSIMD<T_SIMD::AVX2, Unroll>>(size, result.data()); },
                    allocator, "stalker avx2 " + type, logs, config, totalBytes);

        Benchmark::papiRun([&](auto& result) { MemoryOperations::setZero<T, ExecutionTraitSIMD<T_SIMD::AVX512, Unroll>>(size, result.data()); },
                allocator, "stalker avx512 " + type, logs, config, totalBytes);

        config.compareOver = {};
        Benchmark::papiRun([&](auto& result) { MemoryOperations::setZero<T, ExecutionTraitClassic<true>>(size, result.data()); },
                allocator, "memset " + type, logs, config, totalBytes);

        Benchmark::papiRun([&](auto& result) { if constexpr (std::is_same_v<T, float>)
                                                    cblas_sscal(static_cast<int>(size), 0.0f, result.data(), 1);
                                            else if constexpr (std::is_same_v<T, double>)
                                                    cblas_dscal(static_cast<int>(size), 0.0, result.data(), 1);
                                            },
                allocator, "blas " + type, logs, config, totalBytes);
    }

    public:
        Memory() = default;

        void run(size_t iterations,
                std::vector<size_t> sizes, std::array<float, 3> thresholds) {
        };

        void run(){
            auto logs = Logs(std::string("Memory Copy "));


            // int size = 2'000'000'000;
            int size = 2'000'000;
            // int size = 1'000'000;
            // int size = 10'000;
            // int size = 2'000;

            std::ostringstream ss;
            ss << std::scientific << std::setprecision(6) << static_cast<double>(size);
            logs.addParameter("Size", ss.str());
            ss.str(""); ss.clear();
            logs.addParameter("Alignment", DefaultAlignment());
            logs.addParameter("UnrollFactor", DefaultUnroll());

            #if defined(STALKER_BENCH_PAPI_AVAILABLE) && (STALKER_BENCH_PAPI_AVAILABLE)
            logs.addMeasurementSet("GBps", true);
            logs.addMeasurementSet("FP_Ops", false);
            logs.addMeasurementSet("FLOPS", true);
            logs.addMeasurementSet("IPC", true);
            logs.addMeasurementSet("AI [FP_Ops/bytes]", false);
            logs.addMeasurementSet("Instructions", true);
            logs.addMeasurementSet("Cycles", true);
            logs.addMeasurementSet("L2_Misses", true);

            #endif

            #define CALL(T) this->_testCopy<T>(size, logs);
            BENCH_FOR_EACH_TYPE(CALL)   // no Benchmarks:: qualifier
            #undef CALL

            logs.exportToJSON(std::string(std::getenv("HOME")) + "/code/Stalker/Benchmarks/", "exw_eukoilia");

        }

  
    };
};