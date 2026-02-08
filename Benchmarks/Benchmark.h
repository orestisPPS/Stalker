/*
 * Copyright 2024-2026 Papas Christoforos Orestis
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once
#include <Stalker/Utility/Logs.h>
#include <Stalker/Utility/Printers.h>
#include <Stalker/Mathematics/Random.h>
#include <Stalker/Memory/Allocators.h>
#include <Stalker/Core/Config/SIMD.h>

#include <array>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cstring>
#include <cassert>
#include <thread>
#include <pthread.h>
#include <sched.h>
#include <cstdlib>
#include <sstream>
#include "BenchmarkConfig.hpp"   // from ${CMAKE_CURRENT_BINARY_DIR}

#if defined(STALKER_BENCH_PAPI_AVAILABLE) && STALKER_BENCH_PAPI_AVAILABLE
#include <papi.h>
#endif
#if defined(STALKER_BENCH_OPENBLAS_AVAILABLE) && STALKER_BENCH_OPENBLAS_AVAILABLE
#include <cblas.h>
#include <dlfcn.h>
// Use a runtime lookup to avoid requiring a visible definition at analysis time
static inline void try_set_openblas_threads(int n) {
    // Attempt to resolve the symbol in the current process; no-op if not present.
    void* handle = dlopen(nullptr, RTLD_LAZY);
    if (!handle) return;
    using Fn = void(*)(int);
    Fn fn = reinterpret_cast<Fn>(dlsym(handle, "openblas_set_num_threads"));
    if (fn) fn(n);
    dlclose(handle);
}
#else
static inline void try_set_openblas_threads(int) { (void)0; }
#endif
#if defined(STALKER_BENCH_EIGEN_AVAILABLE) && STALKER_BENCH_EIGEN_AVAILABLE
#include <Eigen/Core>
#include <Eigen/src/Core/util/Memory.h>
#ifdef EIGEN_USE_THREADS
#include <unsupported/Eigen/CXX11/ThreadPool>
#endif
#endif

namespace Benchmarks {
using namespace Stalker::Utility;
using namespace Stalker::Core;
namespace fs = std::filesystem;

    /**
     * Prevent the compiler from optimizing away the given value.
     *
     * Acts as an optimization barrier by using inline volatile assembly so
     * the value is considered used without being modified.
     *
     * @tparam Tp Type of the value.
     * @param value Constant reference to the value to preserve.
     */
    template <class Tp>
    inline __attribute__((always_inline)) void doNotOptimize(Tp const& value) {
        asm volatile("" : : "r,m"(value) : "memory");
    }

#if defined(STALKER_BENCH_PAPI_AVAILABLE) && (STALKER_BENCH_PAPI_AVAILABLE)

#endif

    template<typename Child>
    class Benchmark {

    protected:

        Benchmark(const std::string& name, const std::string& logExportPath) :
            _name(name), _logExportPath(logExportPath), _logs(name) {
            assert(_checkDir(logExportPath));
            printTitle("Benchmark Suite: " + name, "=", T_Color::FERRARI_RED);
        }

        std::string _name;
        std::string _logExportPath;
        Logs _logs;

        struct SingleBenchmarkConfig {
            std::string name;
            size_t iterations = Benchmarks::Iterations;      ///< Number of iterations
            size_t warmupIterations = Benchmarks::WarmupIterations;         ///< Number of warmup iterations
            bool verbose = true;                             ///< Verbose output
            std::vector<std::string> compareOver;            ///< Names to compare over in measurement sets
        };

        template<typename TestFuncT>
        void _benchmarkTime(TestFuncT&& func, const SingleBenchmarkConfig& config) {
            printSubtitle(config.name, T_Color::BRIGHT_WHITE);
            for (size_t i = 0; i < config.iterations; ++i) {
                print("  Iteration " + std::to_string(i+1) + "/" + std::to_string(config.iterations), T_Color::GANDALF_GRAY);
                auto timer = Timer();
                timer.start();
                func();
                timer.stop();
                auto stopwatch = _logs.getOrCreateStopwatch(config.name);
                stopwatch.registerTimer(timer);
            }
        }

        template<typename Func, std::size_t... Is>
        void _forEachUnrollImpl(Func&& func, std::index_sequence<Is...>) {
            (func(std::integral_constant<std::size_t, Benchmarks::Unrolls[Is]>{}), ...);
        }

        template<typename Func>
        void _forEachUnroll(Func&& func) {
            _forEachUnrollImpl(std::forward<Func>(func), std::make_index_sequence<Benchmarks::UnrollCount>{});
        }

        template<typename Func>
        void _forEachStorePolicy(Func&& func) {
            #define CALL_FUNC(Policy) func(std::integral_constant<Stalker::Core::Config::T_SIMDStore, Policy>{});
            BENCH_FOR_EACH_STORE_POLICY(CALL_FUNC)
            #undef CALL_FUNC
        }

        template<typename T>
        std::vector<T, Stalker::Memory::AlignedAllocator<T, Stalker::Core::Config::DefaultAlignment()>> _createData(size_t size) {
            if (Benchmarks::Seed != 0) {
                Stalker::Mathematics::Random::setSeed(static_cast<unsigned int>(Benchmarks::Seed));
            }
            auto data = Stalker::Memory::createAlignedVector<T>(size);
            Stalker::Mathematics::Random::uniform<T>(size, data.data(), static_cast<T>(Benchmarks::DataRangeMin), static_cast<T>(Benchmarks::DataRangeMax));
            return data;
        }

        #if defined(STALKER_BENCH_PAPI_AVAILABLE) && STALKER_BENCH_PAPI_AVAILABLE

        struct PAPILogsConfig{
            struct PAPILog{
                bool measure;
                bool plot;
            };
            std::unordered_map<std::string, PAPILog> measurements {
                {"GB/s", {true, true}},
                {"GFLOPS", {true, true}},
                {"IPC", {true, true}},
                {"CPE", {true, true}},
                {"AI [FLOPs/bytes]", {true, false}},
                {"FLOPs", {true, false}},
                {"Instructions", {true, true}},
                {"Cycles", {true, true}},
                {"L2_Misses", {true, true}},
                {"MPKI", {true, true}},
                {"Miss/Element", {true, true}}
            };
        };

        
        struct Metrics {
            double    timeSec{0.0};              ///< Wall-clock time (s)
            long long floatingPointOps{0};       ///< PAPI_FP_OPS (or hint)
            long long instructions{0};           ///< PAPI_TOT_INS
            long long cycles{0};                 ///< PAPI_TOT_CYC
            long long cacheMissesL2{0};          ///< PAPI_L2_DCM

            double    gflops{0.0};               ///< GFLOP/s [FLOPs/s/1E9]
            double    gbps{0.0};                 ///< GB/s [Bytes/s/1E9]
            double    ipc{0.0};                  ///< Instructions per cycle [total instructions / total cycles]
            double    arithmeticIntensity{0.0};  ///< FLOPs per byte
            double    cacheMissesPerElement{0};  ///< L2 Misses per element
            double    MPKI{0};                   ///< L2 Misses per 1000 Instructions


            void printMetrics() const {
                using namespace Stalker::Utility;
                const int labelWidth = 14;
                auto printField = [&](const std::string& label, auto value) {
                    std::ostringstream oss;
                    oss << std::left << std::setw(labelWidth) << label;
                    oss << std::scientific << std::setprecision(3) << value;
                    print(oss.str());
                };
                printField("Time (s):", timeSec);
                printField("Cycles:", static_cast<double>(cycles));
                printField("GFLOPS:", gflops);
                printField("GB/s:", gbps);
                printField("IPC:", ipc);
                printField("AI:", arithmeticIntensity);
                printField("FLOPs:", static_cast<double>(floatingPointOps));
                printField("Instructions:", static_cast<double>(instructions));
                printField("L2 Misses:", static_cast<double>(cacheMissesL2));
                printField("MPKI:", MPKI);
                printField("Miss/Element:", cacheMissesPerElement);
            }
        };



        // bytesTotal: total bytes touched (e.g., vector copy: 2*N*sizeof(T))
        // IF IT FAILS CHECK YOUR PARANOIA LEVELS WITH: cat /proc/sys/kernel/perf_event_paranoid
        // You might need to set it to 1 to enable some system counters: sudo sysctl -w kernel.perf_event_paranoid=1
        template<typename TestFuncT, typename AllocFuncT>
        void _benchmarkPAPI(TestFuncT&& func,
                            AllocFuncT&& allocFunc,
                            const SingleBenchmarkConfig& config,
                            size_t bytesTotal, size_t vectorSize) {

            for (size_t i = 0; i < config.warmupIterations; ++i) {
                print("  Warmup Iteration " + std::to_string(i+1) + "/" + std::to_string(config.warmupIterations), T_Color::GANDALF_GRAY);
                auto result = allocFunc();
                if constexpr (std::is_void_v<decltype(func(result))>) {
                    func(result);
                } else {
                    doNotOptimize(func(result));
                }
                doNotOptimize(result);
            }

            for (size_t i = 0; i < config.iterations; ++i) {
                printSubtitle(config.name + " Iteration " + std::to_string(i+1) + "/" + std::to_string(config.iterations), T_Color::BUTIAS_ORANGE);
                auto timer = Timer();
                Metrics metrics{};
                static bool papiOk = (PAPI_library_init(PAPI_VER_CURRENT) == PAPI_VER_CURRENT);
                int eventSet = PAPI_NULL;
                std::vector<int> events;
                std::vector<long long> vals;

                if (papiOk && PAPI_create_eventset(&eventSet) == PAPI_OK) {
                    auto tryAdd = [&](int ev){
                        if (PAPI_query_event(ev)==PAPI_OK && PAPI_add_event(eventSet, ev)==PAPI_OK)
                            events.push_back(ev);
                    };
                    tryAdd(PAPI_TOT_CYC);
                    tryAdd(PAPI_TOT_INS);
                    tryAdd(PAPI_FP_OPS);
                    tryAdd(PAPI_L2_DCM);

                    if (!events.empty()) PAPI_start(eventSet);
                }

                if (config.verbose) printInfo("Allocating result vector...");
                auto result = allocFunc();
                if (config.verbose) {
                    printInfo(std::string("Executing function (affinity core ") + std::to_string(Benchmarks::SlaveThreadId) + ")...");
                }
                timer.start();
                if constexpr (std::is_void_v<decltype(func(result))>) {
                    func(result);
                } else {
                    doNotOptimize(func(result));
                }
                timer.stop();
                doNotOptimize(result);

                if (papiOk && eventSet != PAPI_NULL) {
                    vals.assign(events.size(), 0);
                    if (!events.empty())
                        PAPI_stop(eventSet, vals.data());
                    PAPI_cleanup_eventset(eventSet);
                    PAPI_destroy_eventset(&eventSet);

                    metrics.timeSec = timer.durationValue();
                    for (size_t i = 0; i < events.size(); ++i) {
                        switch (events[i]) {
                            case PAPI_FP_OPS:
                            metrics.floatingPointOps = vals[i];
                            break;
                            case PAPI_TOT_INS:
                            metrics.instructions  = vals[i];
                            break;
                            case PAPI_TOT_CYC:
                            metrics.cycles = vals[i];
                            break;
                            case PAPI_L2_DCM:  
                            metrics.cacheMissesL2 = vals[i];
                            break;
                        }
                    }
                }
                auto testName = config.name;
                auto& stopwatch = _logs.getOrCreateStopwatch(testName);
                stopwatch.registerTimer(timer);
                stopwatch.getTags() = config.compareOver;

                metrics.gflops                = metrics.floatingPointOps / metrics.timeSec / 1E9;
                metrics.gbps                  = double(bytesTotal) / (metrics.timeSec * 1E9);
                metrics.ipc                   = double(metrics.instructions) / double(metrics.cycles);
                metrics.arithmeticIntensity   = double(metrics.floatingPointOps) / double(bytesTotal);
                metrics.cacheMissesPerElement = double(metrics.cacheMissesL2) / double(vectorSize);
                metrics.MPKI                  = (double(metrics.cacheMissesL2) / double(metrics.instructions)) * 1000.0;
            
                _logs.addMeasurementToSet("FLOPs", testName, metrics.floatingPointOps, config.compareOver);
                _logs.addMeasurementToSet("Instructions", testName, metrics.instructions, config.compareOver);
                _logs.addMeasurementToSet("Cycles", testName, metrics.cycles, config.compareOver);
                _logs.addMeasurementToSet("L2_Misses", testName, metrics.cacheMissesL2, config.compareOver);
                _logs.addMeasurementToSet("GFLOPS", testName, metrics.gflops, config.compareOver);
                _logs.addMeasurementToSet("GB/s", testName, metrics.gbps, config.compareOver);
                _logs.addMeasurementToSet("IPC", testName, metrics.ipc, config.compareOver);
                _logs.addMeasurementToSet("AI [FLOPs/bytes]", testName, metrics.arithmeticIntensity, config.compareOver);
                _logs.addMeasurementToSet("MPKI", testName, metrics.MPKI, config.compareOver);
                _logs.addMeasurementToSet("Miss/Element", testName, metrics.cacheMissesPerElement, config.compareOver);
                if (config.verbose) metrics.printMetrics();
            }   
        }
        #endif

        void _prepareLogs(size_t size, const PAPILogsConfig& papiConfig) {
            std::ostringstream ss;
            ss << std::scientific << std::setprecision(3) << static_cast<double>(size);
            _logs.addParameter("Size", ss.str());
            ss.str(""); ss.clear();

            #if defined(STALKER_BENCH_PAPI_AVAILABLE) && (STALKER_BENCH_PAPI_AVAILABLE)
            auto addIfMeasured = [&](const std::string& key) {
                // if (papiConfig && papiConfig.measurements.at(key).measure)
                if (papiConfig.measurements.at(key).measure)
                    _logs.addMeasurementSet(key, papiConfig.measurements.at(key).plot);
                else
                    throw std::runtime_error("PAPI measurement for key '" + key + "' not configured.");
            };
            addIfMeasured("GB/s");
            addIfMeasured("FLOPs");
            addIfMeasured("GFLOPS");
            addIfMeasured("IPC");
            addIfMeasured("AI [FLOPs/bytes]");
            addIfMeasured("Instructions");
            addIfMeasured("Cycles");
            addIfMeasured("L2_Misses");
            #endif

        }

        static bool _checkDir(const std::string& path) {
            if (path.empty()) return true;

            std::error_code ec;
            const fs::path p(path);

            // Exists?
            if (fs::exists(p, ec)) {
                if (ec) { printError("fs::exists failed: " + ec.message()); return false; }
                if (!fs::is_directory(p, ec)) {
                    printError("Log export path exists but is not a directory: " + path);
                    return false;
                }
                return true; // ok, directory already there
            }

            // Create with parents
            fs::create_directories(p, ec);
            if (!ec) {
                printInfo("Created log export path: " + path);
                return true;
            }

            // If creation failed, report why
            printError("Failed to create log export path '" + path + "': " + ec.message());
            return false;
        }

    public:

        void run() {
            auto suiteBody = [this]() {
                // Configure math library thread counts to match Benchmarks::NumThreads
                if constexpr (true) {
                    const int nth = Benchmarks::NumThreads;
                    // Set OpenBLAS threads through environment for deterministic behavior
                    if (nth > 0) {
                        std::string nthStr = std::to_string(nth);
                        setenv("OPENBLAS_NUM_THREADS", nthStr.c_str(), 1);
                        printInfo(std::string("Set env: OPENBLAS_NUM_THREADS=") + nthStr);
                    }
                    #if defined(STALKER_BENCH_OPENBLAS_AVAILABLE) && STALKER_BENCH_OPENBLAS_AVAILABLE
                    if (nth > 0) try_set_openblas_threads(nth);
                    #endif
                    #if defined(STALKER_BENCH_EIGEN_AVAILABLE) && STALKER_BENCH_EIGEN_AVAILABLE
                    // Eigen's default is single-threaded for plain vectorization; for parallel modules, set global threads
                    if (nth > 0) Eigen::setNbThreads(nth);
                    #endif
                }
                // Pin this std::thread to configured core (SlaveThreadId) if valid
                const unsigned hw = std::thread::hardware_concurrency();
                if (hw == 0) {
                    printWarning("hardware_concurrency() returned 0; skipping affinity pin.");
                } else if (Benchmarks::SlaveThreadId >= static_cast<int>(hw)) {
                    printWarning("Requested slave_thread_id (" + std::to_string(Benchmarks::SlaveThreadId) + 
                                 ") >= hardware_concurrency (" + std::to_string(hw) + ") — skipping affinity pin.");
                } else if (Benchmarks::SlaveThreadId >= 0) {
                    cpu_set_t cpuset;
                    CPU_ZERO(&cpuset);
                    CPU_SET(static_cast<unsigned>(Benchmarks::SlaveThreadId), &cpuset);
                    pthread_t self = pthread_self();
                    int rc = pthread_setaffinity_np(self, sizeof(cpu_set_t), &cpuset);
                    if (rc != 0) {
                        printWarning("pthread_setaffinity_np failed (rc=" + std::to_string(rc) + ")");
                    } else {
                        printInfo("Suite affinity set to core " + std::to_string(Benchmarks::SlaveThreadId));
                    }
                }
                auto timer = Timer();
                timer.start();
                
                static_cast<Child*>(this)->execute();

                timer.stop();
                printTitle("Benchmark Suite '" + _name + "' completed in " + std::to_string(timer.durationValue(TimeUnit::minutes)) + " minutes.", "=");
            };

            std::thread t(suiteBody);
            t.join();
        }

    private:
        // No pinning / affinity: std::thread isolation only.
    };
}; // namespace Benchmarks