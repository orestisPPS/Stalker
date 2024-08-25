#cmake_minimum_required(VERSION 3.27)
#project(stalker)
#
#set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2 -mfma -O0 ")
#
#add_executable(stalker main.cpp
#        Tests/NumericalVectorTest.h
#        Tests/TestBase.h
#        Tests/STLKR_PerformanceTests/STLKR_PerformanceTestBase.h
#        Tests/STLKR_TestBase.cpp
#        Tests/STLKR_TestBase.h
#        Tests/STLKR_PerformanceTests/STLKR_PerformanceTestBase.cpp
#        Tests/StalkerVector_ValidityTests.h
#        Tests/STLKR_PerformanceTests/StalkerVector/StalkerVector_PerformanceTests.h
#        Tests/STLKR_PerformanceTests/StalkerVector/SingleThreadPerformance/SingleThreadOperationsPerformanceTest.h
#        
#        Utility/Timer.cpp
#        Utility/Timer.h
#        Utility/Logs.cpp
#        Utility/Units/STLKR_Units.h
#        
#        Threading/CPUTopology/CPU.h
#        Threading/CPUTopology/Core.h
#        Threading/CPUTopology/Cache.h
#        Threading/CPUTopology/Thread.h
#        Threading/CPUTopology/SharedCache.h
#        Threading/CPUTopology/CPU_Manager.h
#        Threading/ThreadJob.h
#        Threading/ThreadOperations.h
#        
#
#        StalkerVector/StalkerVector.h 
#        StalkerVector/StalkerVector_Iterator.h
#        StalkerVector/AVX2/MathTraits.h
#        StalkerVector/AVX2/MemoryTraits.h
#        StalkerVector/AVX2/Prefetch.h
#        StalkerVector/AVX2/Operations.h
#        Threading/CPUTopology/Cache.h
#)

cmake_minimum_required(VERSION 3.27)
project(stalker)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2 -mfma -O0")

# Define the stalker_core header-only library
add_library(stalker_core INTERFACE)

# Add include directories to the stalker_core library
target_include_directories(stalker_core INTERFACE
        ${PROJECT_SOURCE_DIR}/StalkerVector
        ${PROJECT_SOURCE_DIR}/Threading
        ${PROJECT_SOURCE_DIR}/Utility
)
# Define the stalker_tests library
add_library(stalker_tests STATIC
        Tests/TestBase.h
        Tests/STLKR_PerformanceTests/STLKR_PerformanceTestBase.h
        Tests/STLKR_TestBase.cpp
        Tests/STLKR_TestBase.h
        Tests/STLKR_PerformanceTests/STLKR_PerformanceTestBase.cpp
        Tests/StalkerVector_ValidityTests.h
        Tests/STLKR_PerformanceTests/StalkerVector/StalkerVector_PerformanceTests.h
        Tests/STLKR_PerformanceTests/StalkerVector/SingleThreadPerformance/SingleThreadOperationsPerformanceTest.h
        Tests/CPUTopologyTest.h
)

# Link the stalker_tests library with stalker_core
target_link_libraries(stalker_tests PRIVATE stalker_core)

# Add the main executable
add_executable(stalker main.cpp
        Utility/Readers.h)

# Link the executable with stalker_core and stalker_tests
target_link_libraries(stalker stalker_core stalker_tests)

