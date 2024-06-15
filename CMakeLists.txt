cmake_minimum_required(VERSION 3.27)
project(stalker)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2 -mfma -O0")

add_executable(stalker main.cpp
        SpatialDiscretization/Node.h
        SpatialDiscretization/CurvilinearMesh.cpp
        SpatialDiscretization/CurvilinearMesh.h
        SpatialDiscretization/MeshSpecs.cpp
        SpatialDiscretization/MeshSpecs.h
        SpatialDiscretization/MeshFactory.h
        STLKR_HighPerformance/STLKR_Threading/STLKR_Thread_Operations.h
        Tests/NumericalVectorTest.h
        LinearAlgebra/ContiguousMemoryStorage/VectorOperations.h
        LinearAlgebra/ContiguousMemoryStorage/Norms.cpp
        LinearAlgebra/ContiguousMemoryStorage/Norms.h
        LinearAlgebra/ContiguousMemoryStorage/STLKR_EnumerableMathematicalOperations/STLKR_EnumerableAlgebraicOperations.h
        LinearAlgebra/ContiguousMemoryStorage/STLKR_EnumerableMathematicalOperations/STLKR_Operations.h
        LinearAlgebra/STLKR_ArithmeticTypes.h
        STLKR_HighPerformance/STLKR_SIMD/STLKR_Operations_SIMD.h
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
        STLKR_HighPerformance/STLKR_SIMD/STLKR_Config_SIMD.h
        STLKR_HighPerformance/STLKR_SIMD/STLKR_SIMD_MemoryManager.h
        STLKR_HighPerformance/STLKR_Threading/STLKR_Thread_CacheScheduler.h
        STLKR_HighPerformance/STLKR_Threading/STLKR_Thread_CacheLineInstance.h
        STLKR_HighPerformance/STKR_MachineSetup/STLKR_Machine_CPUEntitiesFactoryLinux.cpp
        STLKR_HighPerformance/STKR_MachineSetup/STLKR_Machine_CPUEntitiesFactoryLinux.h
        STLKR_HighPerformance/STKR_MachineSetup/STLKR_Machine_Core.cpp
        STLKR_HighPerformance/STKR_MachineSetup/STLKR_Machine_Core.h
        STLKR_HighPerformance/STKR_MachineSetup/STLKR_Machine_CacheLevel.cpp
        STLKR_HighPerformance/STKR_MachineSetup/STLKR_Machine_CacheLevel.h
        STLKR_HighPerformance/STKR_MachineSetup/STLKR_Machine_Thread.cpp
        STLKR_HighPerformance/STKR_MachineSetup/STLKR_Machine_Thread.h
)
