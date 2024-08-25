//
// Created by hal9000 on 8/24/24.
//

#ifndef STALKER_CPUTOPOLOGYTEST_H
#define STALKER_CPUTOPOLOGYTEST_H

#include "STLKR_TestBase.h"

namespace STLKR_Tests {

    class CPUTopologyTest : public STLKR_TestBase {
    public:
        explicit CPUTopologyTest() : STLKR_TestBase("CPU Topology Test") {}

        void runTest() override {
            _testReaders();
            _runTopologyTest();
        }

    protected:

         void _testReaders() {
             // Create test files for Readers::readIntegerFromFile
             std::ofstream intFile("test_int.txt");
             intFile << "1234";
             intFile.close();

             std::ofstream emptyIntFile("empty_int.txt");
             emptyIntFile.close();

             // Create test files for Readers::readStringFromFile
             std::ofstream strFile("test_str.txt");
             strFile << "Hello, World!";
             strFile.close();

             std::ofstream emptyStrFile("empty_str.txt");
             emptyStrFile.close();
             resetCaseCounting();
             // Testing Readers::readIntegerFromFile
             STLKR_TestBase::printTestCaseResult(Readers::readIntegerFromFile("test_int.txt") == 1234, "valid integer");
             STLKR_TestBase::printTestCaseResult(Readers::readIntegerFromFile("empty_int.txt") != 0, "empty file");

             // Testing Readers::readStringFromFile
             STLKR_TestBase::printTestCaseResult(Readers::readStringFromFile("test_str.txt") == "Hello, World!", "valid string");
             STLKR_TestBase::printTestCaseResult(Readers::readStringFromFile("empty_str.txt").empty(), "empty file");

             // Testing Readers::parseCPUList
             STLKR_TestBase::printTestCaseResult(Readers::parseCPUList("0-7") == std::vector<unsigned>({0, 1, 2, 3, 4, 5, 6, 7}), "range '0-7'");
             STLKR_TestBase::printTestCaseResult(Readers::parseCPUList("0,2,4,6-7") == std::vector<unsigned>({0, 2, 4, 6, 7}), "mixed '0,2,4,6-7'");
             STLKR_TestBase::printTestCaseResult(Readers::parseCPUList("0") == std::vector<unsigned>({0}), "single '0'");

             // Clean up
             std::remove("test_int.txt");
             std::remove("empty_int.txt");
             std::remove("test_str.txt");
             std::remove("empty_str.txt");
         }

        void testCoreIds(const std::vector<Core*>& cores) {
            for (const auto& core : cores) {
                unsigned coreId = core->getId();
                std::string path = "/sys/devices/system/cpu/cpu" + std::to_string(coreId) + "/topology/core_id";
                unsigned expectedCoreId = Readers::readIntegerFromFile(path);
                STLKR_TestBase::printTestCaseResult(coreId == expectedCoreId, "core id " + std::to_string(coreId) + " matches system");
            }
        }

        void testThreadIds(const std::vector<Core*>& cores) {
            for (const auto& core : cores) {
                unsigned coreId = core->getId();
                const auto& threads = core->getThreads();

                // Iterate through each thread in the core
                for (const auto& thread : threads) {
                    unsigned threadId = thread->getId();
                    std::string path = "/sys/devices/system/cpu/cpu" + std::to_string(threadId) + "/topology/core_id";
                    unsigned expectedCoreId = Readers::readIntegerFromFile(path);

                    // Validate that the thread belongs to the correct core
                    STLKR_TestBase::printTestCaseResult(coreId == expectedCoreId, "thread " + std::to_string(threadId) + " belongs to core " + std::to_string(coreId));

                    // Validate the thread siblings (hyperthreading or multi-threading)
                    std::string siblingsPath = "/sys/devices/system/cpu/cpu" + std::to_string(threadId) + "/topology/thread_siblings_list";
                    std::vector<unsigned> expectedThreadIds = Readers::parseCPUList(Readers::readStringFromFile(siblingsPath));

                    STLKR_TestBase::printTestCaseResult(std::find(expectedThreadIds.begin(), expectedThreadIds.end(), threadId) != expectedThreadIds.end(),
                                                        "thread " + std::to_string(threadId) + " siblings list includes itself");

                    STLKR_TestBase::printTestCaseResult(threads.size() == expectedThreadIds.size(),
                                                        "core " + std::to_string(coreId) + " thread count matches system for thread " + std::to_string(threadId));
                }
            }
        }


        void testThreadClockFrequencies(const std::vector<Core*>& cores) {
            for (const auto& core : cores) {
                for (const auto& thread : core->getThreads()) {
                    unsigned threadId = thread->getId();
                    std::string minFreqPath = "/sys/devices/system/cpu/cpu" + std::to_string(threadId) + "/cpufreq/cpuinfo_min_freq";
                    std::string maxFreqPath = "/sys/devices/system/cpu/cpu" + std::to_string(threadId) + "/cpufreq/cpuinfo_max_freq";
                    unsigned expectedMinClock = Readers::readIntegerFromFile(minFreqPath);
                    unsigned expectedMaxClock = Readers::readIntegerFromFile(maxFreqPath);

                    STLKR_TestBase::printTestCaseResult(thread->getClockMin() == expectedMinClock, "thread " + std::to_string(threadId) + " min clock matches system");
                    STLKR_TestBase::printTestCaseResult(thread->getClockMax() == expectedMaxClock, "thread " + std::to_string(threadId) + " max clock matches system");
                }
            }
        }

        void testCacheLevels(const std::vector<CacheLevel*>& cacheLevels) {
            for (const auto& cacheLevel : cacheLevels) {
                unsigned cacheLevelType = cacheLevel->getLevel();
                unsigned cacheSize = cacheLevel->getSize();
                const auto& sharedCPUs = cacheLevel->getThreads();

                bool found = false;
                for (unsigned cpu : sharedCPUs) {
                    std::string sizePath = "/sys/devices/system/cpu/cpu" + std::to_string(cpu) + "/cache/index" + std::to_string(cacheLevelType) + "/size";
                    std::string sharedCpuPath = "/sys/devices/system/cpu/cpu" + std::to_string(cpu) + "/cache/index" + std::to_string(cacheLevelType) + "/shared_cpu_list";
                    unsigned expectedSize = Readers::readIntegerFromFile(sizePath);
                    std::vector<unsigned> expectedSharedCPUs = Readers::parseCPUList(Readers::readStringFromFile(sharedCpuPath));

                    if (cacheSize == expectedSize && sharedCPUs == expectedSharedCPUs) {
                        found = true;
                        STLKR_TestBase::printTestCaseResult(true, "cache level " + std::to_string(cacheLevelType) + " size and shared CPUs match system");
                        break;
                    }
                }
                if (!found) {
                    STLKR_TestBase::printTestCaseResult(false, "cache level " + std::to_string(cacheLevelType) + " does not match system data");
                }
            }
        }

        void _runTopologyTest() {
            _manager.setAvailableCores(_manager.getTotalCores());
            auto cores = _manager.getCores();
            testCoreIds(cores);
            testThreadIds(cores);
            testThreadClockFrequencies(cores);
            _manager.release(cores);
            testCacheLevels(_manager.getCacheLevels());
        }
    };
}
#endif //STALKER_CPUTOPOLOGYTEST_H
