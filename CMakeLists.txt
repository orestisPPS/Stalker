cmake_minimum_required(VERSION 3.27)
project(stalker)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2 -mfma -O0 ")

add_executable(stalker main.cpp
        
        Tests/NumericalVectorTest.h
        LinearAlgebra/ContiguousMemoryStorage/Norms.cpp
        LinearAlgebra/ContiguousMemoryStorage/Norms.h
        Tests/TestBase.h
        Utility/Timer.cpp
        Utility/Timer.h
        Units/STLKR_Units.h
        Utility/Logs.cpp
        Tests/STLKR_PerformanceTests/STLKR_PerformanceTestBase.h
        Tests/STLKR_TestBase.cpp
        Tests/STLKR_TestBase.h
        Tests/STLKR_PerformanceTests/STLKR_PerformanceTestBase.cpp
        MachineTopology/CPUTopologyLinux.h
        MachineTopology/Core.h
        MachineTopology/CacheLevel.h
        MachineTopology/Thread.h
        MachineTopology/SharedCache.h
        Operations/Thread_Operations.h
        Operations/ThreadJob.h
        MachineTopology/CPU_Manager.h
        LinearAlgebra/ContiguousMemoryStorage/StalkerVector.h
        Tests/StalkerVector_ValidityTests.h
        LinearAlgebra/ContiguousMemoryStorage/ContiguousMemoryIterator.h
        Operations/SingleInstructionMultipleData/AVX_MemoryTraits.h
        Operations/SingleInstructionMultipleData/SIMD_MathTraits.h
        Tests/STLKR_PerformanceTests/StalkerVector/StalkerVector_PerformanceTests.h
        Tests/STLKR_PerformanceTests/StalkerVector/SingleThreadPerformance/SingleThreadOperationsPerformanceTest.h
        Operations/SingleInstructionMultipleData/Prefetch_Operations.h
        Operations/SingleInstructionMultipleData/SIMD_Operations.h
        Utility/ConstexprConverter.h
        Operations/SingleInstructionMultipleData/SIMD_Traits.h
)
