//
// Created by hal9000 on 6/29/24.
//

#ifndef STALKER_THREADINGPERFORMANCETEST_H
#define STALKER_THREADINGPERFORMANCETEST_H

#include <cassert>
#include "STLKR_PerformanceTestBase.h"
#include "../../Utility/Timer.h"
#include "../../STLKR_HighPerformance/STLKR_SIMD/STLKR_Operations_SIMD.h"
#include "../../STLKR_HighPerformance/STLKR_MachineSetup/STLKR_Machine_CPUTopologyLinux.h"

namespace STLKR_Tests{
    
    template<size_t size, size_t physicalCores = 2>
    class ThreadingPerformanceTest : public STLKR_PerformanceTestBase {
    public:
        ThreadingPerformanceTest(const std::string &path) :
            STLKR_PerformanceTestBase("ThreadingPerformanceTest", path),
            _data1(nullptr),
            _data2(nullptr),
            _cpuTopology(STLKR_Machine_CPUTopologyLinux()){

        }
        
        void runTest() override {
            logs.addParameter("size", std::to_string(size));
            logs.addParameter("alignment", 64);
            logs.addParameter("physical_cpus", std::to_string(physicalCores));
            
            size_t totalThreads = 0;
            for (const auto &core : _cpuTopology.getPhysicalCores()) {
                core->setHyperThreading(true);
                totalThreads += core->getThreads().size();
            }
            logs.addParameter("logical_cpus", std::to_string(totalThreads));
            
            //Non Aligned Data
            _data1 = new double[size];
            _data2 = new double[size];
            for (size_t i = 0; i < size; i++) {
                _data1[i] = static_cast<double>(i);
                _data2[i] = static_cast<double>(i);
            }
            stdSingleThreadAddition(false);
            //hyper-threading enabled
            multiThreadAddition(physicalCores, true, false);
            //hyper-threading disabled
            multiThreadAddition(physicalCores, false, false);
            
            //Aligned Data
            delete[] _data1;
            delete[] _data2;
            _data1 = static_cast<double*>(_mm_malloc(size * sizeof(double), 64));
            _data2 = static_cast<double*>(_mm_malloc(size * sizeof(double), 64));
        
            for (size_t i = 0; i < size; i++) {
                _data1[i] = static_cast<double>(i);
                _data2[i] = static_cast<double>(i);
            }
            stdSingleThreadAddition(true);
            //hyper-threading enabled
            multiThreadAddition(physicalCores, true, true);
            //hyper-threading disabled
            multiThreadAddition(physicalCores, false, true);
            
            logs.exportToCSV(_path, "ThreadingTest");
            logs.clearAllLogs();
            _mm_free(_data1);
            _mm_free(_data2);
        }
    private:
        STLKR_Machine_CPUTopologyLinux _cpuTopology;
        double* _data1;
        double* _data2;

        
        void stdSingleThreadAddition(bool aligned = false) {
            auto result = aligned ? static_cast<double*>(_mm_malloc(size * sizeof(double), 64)) : new double[size];
            auto name = "std_addition_threads_1_" + boolToOnOff(aligned, "alignment");
            logs.startSingleObservationTimer(name, STLKR_TimeUnit::microseconds);
            for (size_t i = 0; i < size; i++) {
                result[i] = _data1[i] + _data2[i];
            }
            logs.stopSingleObservationTimer(name);
            delete[] result;
        }

        void multiThreadAddition(unsigned  numCores, bool enableHyperThreading, bool aligned) {
            auto result = aligned ? static_cast<double*>(_mm_malloc(size * sizeof(double), 64)) : new double[size];
            assert(numCores <= _cpuTopology.getPhysicalCores().size());
            auto additionJob = [&](unsigned startIndex, unsigned endIndex) {
                for (unsigned i = startIndex; i < endIndex; i++) {
                    result[i] = _data1[i] + _data2[i];
                }
            };
            auto machineCores = _cpuTopology.getPhysicalCores();
            auto availableCores = std::vector<STLKR_Machine_Core*>(machineCores.begin(), machineCores.begin() + numCores);
            auto name = "add_threads_" + std::to_string(numCores) +"_" +
                                  boolToOnOff(enableHyperThreading, "hyperThreading") + boolToOnOff(aligned, "alignment");
            logs.startSingleObservationTimer(name, STLKR_TimeUnit::microseconds);
            STLKR_Thread_OperationsLinux::executeJob(additionJob, size, availableCores, enableHyperThreading);
            logs.stopSingleObservationTimer(name);
            
            for (size_t i = 0; i < size; i++) {
                if (result[i] != _data1[i] + _data2[i]) {
                    std::cerr << "Error at index: " << i <<"Expected: " << _data1[i] + _data2[i] << "Computed: " << result[i] << std::endl;
                    break;
                }
            }
            delete[] result;
        }
        
    };
}
#endif //STALKER_THREADINGPERFORMANCETEST_H
