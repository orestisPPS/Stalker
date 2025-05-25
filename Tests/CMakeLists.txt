add_executable(StalkerTestRunner main.cpp)
target_include_directories(StalkerTestRunner PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(StalkerTestRunner PRIVATE StalkerCore StalkerMemory StalkerUtility StalkerMathematics) # Remove StalkerMathematics unless you define it as INTERFACE

enable_testing()
add_test(NAME StalkerTestRunner COMMAND StalkerTestRunner)
