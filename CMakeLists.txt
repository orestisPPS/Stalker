cmake_minimum_required(VERSION 3.27)
project(stalker)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2 -mfma -O3")

add_executable(stalker main.cpp
        SpatialDiscretization/Node.h
        SpatialDiscretization/CurvilinearMesh.cpp
        SpatialDiscretization/CurvilinearMesh.h
        SpatialDiscretization/MeshSpecs.cpp
        SpatialDiscretization/MeshSpecs.h
        SpatialDiscretization/MeshFactory.h
        LinearAlgebra/ContiguousMemoryStorage/NumericalVector.h
        ThreadingOperations/STLKR_ThreadingOperations.h
        Tests/NumericalVectorTest.h
        LinearAlgebra/ContiguousMemoryStorage/VectorOperations.h
        LinearAlgebra/ContiguousMemoryStorage/Norms.cpp
        LinearAlgebra/ContiguousMemoryStorage/Norms.h
        LinearAlgebra/ContiguousMemoryStorage/STLKR_EnumerableMathematicalOperations/STLKR_EnumerableAlgebraicOperations.h
        LinearAlgebra/ContiguousMemoryStorage/STLKR_EnumerableMathematicalOperations/STLKR_Operations.h
        LinearAlgebra/STLKR_ArithmeticTypes.h
        LinearAlgebra/ContiguousMemoryStorage/STLKR_EnumerableMathematicalOperations/STLKR_Operations_SIMD.h
        LinearAlgebra/ContiguousMemoryStorage/STLKR_NumericalVector.h
        LinearAlgebra/ContiguousMemoryStorage/STLKR_Enumerable/STLKR_EnumerableBase.h
        LinearAlgebra/ContiguousMemoryStorage/STLKR_Enumerable/STLKR_Array/STLKR_Array.h
        LinearAlgebra/ContiguousMemoryStorage/STLKR_Enumerable/STLKR_Array/STLKR_Array_SIMD.h
        LinearAlgebra/ContiguousMemoryStorage/STLKR_Enumerable/STLKR_Vector/STLKR_Vector.h
        LinearAlgebra/ContiguousMemoryStorage/STLKR_Enumerable/STLKR_Vector/STLKR_Vector_SIMD.h
        LinearAlgebra/ContiguousMemoryStorage/STLKR_Enumerable/STLKR_SIMD/STLKR_SIMD.h
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
)
