add_executable(Tests main.cpp)
target_link_libraries(Tests PRIVATE Stalker)
target_include_directories(Tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)