#pragma once
#include <string>
#include <vector>
#include <Stalker/Utility/Logs.h>
#include <Stalker/Utility/Printers.h>

#if defined(STALKER_BENCH_PAPI_AVAILABLE) && STALKER_BENCH_PAPI_AVAILABLE
#include <papi.h>
#endif

namespace Benchmarks {
using namespace Stalker::Utility;

    struct Benchmark {

        Benchmark() = default;

        template<typename TestFuncT>
        static void run(TestFuncT&& func, const std::string& testName, Logs& logs, size_t iterations = 5) {
            printSubtitle(testName, T_Color::BRIGHT_WHITE);
            for (size_t i = 0; i < iterations; ++i) {
                print("  Iteration " + std::to_string(i+1) + "/" + std::to_string(iterations), T_Color::GANDALF_GRAY);
                auto timer = Timer();
                timer.start();
                func();
                timer.stop();
                auto stopwatch = logs.getOrCreateStopwatch(testName);
                stopwatch.registerTimer(timer);
            }
        }

        #if defined(STALKER_BENCH_PAPI_AVAILABLE) && STALKER_BENCH_PAPI_AVAILABLE
        
        struct Metrics {
            double timeSec{0.0};              ///< Wall-clock time (s)
            double flops{0.0};               ///< FLOP/s
            double gbps{0.0};                 ///< GB/s
            double ipc{0.0};                  ///< Instructions per cycle
            double arithmeticIntensity{0.0};  ///< FLOPs per byte
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

        struct PAPIConfig {
            size_t iterations = 4;      ///< Number of iterations
            bool verbose = true;        ///< Verbose output
            std::vector<std::string> compareOver;  ///< Names to compare over in measurement sets

        };

        // bytesTotal: total bytes touched (e.g., vector copy: 2*N*sizeof(T))
        // flopsHint : known FLOPs (optional); if 0, uses PAPI_FP_OPS if available
        // IF IT FAILS CHECK YOUR PARANOIA LEVELS WITH: cat /proc/sys/kernel/perf_event_paranoid
        // You might need to set it to 1 to enable some system counters: sudo sysctl -w kernel.perf_event_paranoid=1
        template<typename TestFuncT, typename AllocFuncT>
        static void papiRun(TestFuncT&& func,
                            AllocFuncT&& allocFunc,
                            const std::string& testName,
                            Logs& logs,
                            const PAPIConfig& config,
                            std::size_t bytesTotal = 0) {

            for (size_t i = 0; i < config.iterations; ++i) {
                printSubtitle(testName + " Iteration " + std::to_string(i+1) + "/" + std::to_string(config.iterations), T_Color::BUTIAS_ORANGE);
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
                
                auto& stopwatch = logs.getOrCreateStopwatch(testName);
                stopwatch.registerTimer(timer);
                stopwatch.getTags() = config.compareOver;
                logs.addMeasurementToSet("GBps", testName, metrics.gbps, config.compareOver);
                logs.addMeasurementToSet("FP_Ops", testName, metrics.floatingPointOps, config.compareOver);
                logs.addMeasurementToSet("FLOPS", testName, metrics.flops, config.compareOver);
                logs.addMeasurementToSet("IPC", testName, metrics.ipc, config.compareOver);
                logs.addMeasurementToSet("AI [FP_Ops/bytes]", testName, metrics.arithmeticIntensity, config.compareOver);
                logs.addMeasurementToSet("Instructions", testName, metrics.instructions, config.compareOver);
                logs.addMeasurementToSet("Cycles", testName, metrics.cycles, config.compareOver);
                logs.addMeasurementToSet("L2_Misses", testName, metrics.cacheMissesL2, config.compareOver);
                if (config.verbose) metrics.printMetrics();
            }
        }
        #endif

    };
} // namespace Benchmarks