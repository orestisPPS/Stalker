cmake_minimum_required(VERSION 3.27)
project(stalker)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2 -mfma -O0 ")

add_executable(stalker main.cpp
        SpatialDiscretization/Node.h
        SpatialDiscretization/CurvilinearMesh.cpp
        SpatialDiscretization/CurvilinearMesh.h
        SpatialDiscretization/MeshSpecs.cpp
        SpatialDiscretization/MeshSpecs.h
        SpatialDiscretization/MeshFactory.h
        Tests/NumericalVectorTest.h
        LinearAlgebra/ContiguousMemoryStorage/Norms.cpp
        LinearAlgebra/ContiguousMemoryStorage/Norms.h
        LinearAlgebra/STLKR_ArithmeticTypes.h
        Operations/AVX_Operations.h
        Tests/STLKR_PerformanceTests/AVX_PerformanceTest.h
        Tests/TestBase.h
        Utility/Timer.cpp
        Utility/Timer.h
        Units/STLKR_Units.h
        Utility/Logs.cpp
        Tests/STLKR_PerformanceTests/STLKR_PerformanceTestBase.h
        Tests/STLKR_TestBase.cpp
        Tests/STLKR_TestBase.h
        Tests/STLKR_PerformanceTests/STLKR_PerformanceTestBase.cpp
        Operations/AVX_MemoryManagement.h
        MachineTopology/CPUTopologyLinux.cpp
        MachineTopology/CPUTopologyLinux.h
        MachineTopology/Core.cpp
        MachineTopology/Core.h
        MachineTopology/CacheLevel.cpp
        MachineTopology/CacheLevel.h
        MachineTopology/Thread.cpp
        MachineTopology/Thread.h
        MachineTopology/SharedCache.cpp
        MachineTopology/SharedCache.h
        Operations/Thread_Operations.h
        Operations/JobArguments.h
        Tests/STLKR_PerformanceTests/ThreadingPerformanceTest.h
        Operations/CPU_Manager.cpp
        Operations/CPU_Manager.h
        LinearAlgebra/ContiguousMemoryStorage/StalkerPerformanceVector.h
        Tests/StalkerPerformanceVectorTest.h
        LinearAlgebra/ContiguousMemoryStorage/ContiguousMemoryIterator.h
)
