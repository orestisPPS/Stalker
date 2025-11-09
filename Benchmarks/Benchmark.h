#pragma once
#include <Stalker/Utility/Logs.h>
#include <Stalker/Utility/Printers.h>
#include <cassert>
#include "BenchmarkConfig.hpp"   // from ${CMAKE_CURRENT_BINARY_DIR}

#if defined(STALKER_BENCH_PAPI_AVAILABLE) && STALKER_BENCH_PAPI_AVAILABLE
#include <papi.h>
#endif

namespace Benchmarks {
using namespace Stalker::Utility;
using namespace Stalker::Core;
namespace fs = std::filesystem;

#if defined(STALKER_BENCH_PAPI_AVAILABLE) && (STALKER_BENCH_PAPI_AVAILABLE)

#endif

    template<typename Child>
    class Benchmark {

    protected:

        Benchmark(const std::string& name, const std::string& logExportPath) :
            _name(name), _logs(name), _logExportPath(logExportPath) {
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

        #if defined(STALKER_BENCH_PAPI_AVAILABLE) && STALKER_BENCH_PAPI_AVAILABLE

        struct PAPILogsConfig{
            struct PAPILog{
                bool measure;
                bool plot;
            };
            std::unordered_map<std::string, PAPILog> measurements {
                {"GBps", {true, true}},
                {"FLOPS", {true, true}},
                {"IPC", {true, true}},
                {"AI [FP_Ops/bytes]", {true, false}},
                {"FP_Ops", {true, false}},
                {"Instructions", {true, true}},
                {"Cycles", {true, true}},
                {"L2_Misses", {true, true}}
            };
        };

        
        struct Metrics {
            double    timeSec{0.0};              ///< Wall-clock time (s)
            double    flops{0.0};               ///< FLOP/s
            double    gbps{0.0};                 ///< GB/s
            double    ipc{0.0};                  ///< Instructions per cycle
            double    arithmeticIntensity{0.0};  ///< FLOPs per byte
            long long floatingPointOps{0};    ///< PAPI_FP_OPS (or hint)
            long long instructions{0};        ///< PAPI_TOT_INS
            long long cycles{0};              ///< PAPI_TOT_CYC
            long long cacheMissesL2{0};       ///< PAPI_L2_DCM

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
                printField("FLOPS:", flops);
                printField("GBPS:", gbps);
                printField("IPC:", ipc);
                printField("AI:", arithmeticIntensity);
                printField("FLOPs:", static_cast<double>(floatingPointOps));
                printField("Instructions:", static_cast<double>(instructions));
                printField("Cycles:", static_cast<double>(cycles));
                printField("L2 Misses:", static_cast<double>(cacheMissesL2));
            }
        };



        // bytesTotal: total bytes touched (e.g., vector copy: 2*N*sizeof(T))
        // flopsHint : known FLOPs (optional); if 0, uses PAPI_FP_OPS if available
        // IF IT FAILS CHECK YOUR PARANOIA LEVELS WITH: cat /proc/sys/kernel/perf_event_paranoid
        // You might need to set it to 1 to enable some system counters: sudo sysctl -w kernel.perf_event_paranoid=1
        template<typename TestFuncT, typename AllocFuncT>
        void _benchmarkPAPI(TestFuncT&& func,
                            AllocFuncT&& allocFunc,
                            const SingleBenchmarkConfig& config,
                            std::size_t bytesTotal = 0) {
            for (size_t i = 0; i < config.warmupIterations; ++i) {
                print("  Warmup Iteration " + std::to_string(i+1) + "/" + std::to_string(config.warmupIterations), T_Color::GANDALF_GRAY);
                auto result = allocFunc();
                func(result);
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
                if (config.verbose) printInfo("Executing function...");
                timer.start();
                func(result);
                timer.stop();

                metrics.timeSec = timer.durationValue();
                if (bytesTotal > 0)
                metrics.gbps   = double(bytesTotal) / metrics.timeSec / 1e9;

                if (papiOk && eventSet != PAPI_NULL) {
                    vals.assign(events.size(), 0);
                    if (!events.empty()) PAPI_stop(eventSet, vals.data());
                    PAPI_cleanup_eventset(eventSet);
                    PAPI_destroy_eventset(&eventSet);

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

                metrics.flops = metrics.floatingPointOps / metrics.timeSec;
                metrics.ipc = double(metrics.instructions) / double(metrics.cycles);
                
                
                if (bytesTotal > 0 && metrics.floatingPointOps)
                    metrics.arithmeticIntensity = double(metrics.floatingPointOps) / double(bytesTotal);
                auto testName = config.name;
                auto& stopwatch = _logs.getOrCreateStopwatch(testName);
                stopwatch.registerTimer(timer);
                stopwatch.getTags() = config.compareOver;
                _logs.addMeasurementToSet("GBps", testName, metrics.gbps, config.compareOver);
                _logs.addMeasurementToSet("FP_Ops", testName, metrics.floatingPointOps, config.compareOver);
                _logs.addMeasurementToSet("FLOPS", testName, metrics.flops, config.compareOver);
                _logs.addMeasurementToSet("IPC", testName, metrics.ipc, config.compareOver);
                _logs.addMeasurementToSet("AI [FP_Ops/bytes]", testName, metrics.arithmeticIntensity, config.compareOver);
                _logs.addMeasurementToSet("Instructions", testName, metrics.instructions, config.compareOver);
                _logs.addMeasurementToSet("Cycles", testName, metrics.cycles, config.compareOver);
                _logs.addMeasurementToSet("L2_Misses", testName, metrics.cacheMissesL2, config.compareOver);
                if (config.verbose) metrics.printMetrics();
            }
        }
        #endif

        void _clearAndResetLogs(size_t size, size_t unroll, const PAPILogsConfig& papiConfig) {
           _logs.clear();
           std::ostringstream ss;
            ss << std::scientific << std::setprecision(3) << static_cast<double>(size);
            _logs.addParameter("Size", ss.str());
            ss.str(""); ss.clear();
            _logs.addParameter("Unroll", std::to_string(unroll));

            #if defined(STALKER_BENCH_PAPI_AVAILABLE) && (STALKER_BENCH_PAPI_AVAILABLE)
            auto addIfMeasured = [&](const std::string& key) {
                // if (papiConfig && papiConfig.measurements.at(key).measure)
                if (papiConfig.measurements.at(key).measure)
                    _logs.addMeasurementSet(key, papiConfig.measurements.at(key).plot);
                else
                    throw std::runtime_error("PAPI measurement for key '" + key + "' not configured.");
            };
            addIfMeasured("GBps");
            addIfMeasured("FP_Ops");
            addIfMeasured("FLOPS");
            addIfMeasured("IPC");
            addIfMeasured("AI [FP_Ops/bytes]");
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
            auto timer = Timer();
            timer.start();
            for (size_t sIndex = 0; sIndex < Benchmarks::Sizes.size(); ++sIndex) {
                    size_t size = Benchmarks::Sizes[sIndex];
                    auto papiConfig = PAPILogsConfig{};                    
        
                    #define CALL_UNROLL(U) static_cast<Child*>(this)->template _run<U>(size);
                    BENCH_FOR_EACH_UNROLL(CALL_UNROLL)
                    #undef CALL_UNROLL
            }
            timer.stop();
            printTitle("Benchmark Suite '" + _name + "' completed in " +
                       std::to_string(timer.durationValue(TimeUnit::minutes)) + " minutes.", "=");
            
        }
    };
}; // namespace Benchmarks