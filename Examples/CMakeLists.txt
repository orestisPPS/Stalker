cmake_minimum_required(VERSION 3.18)

add_custom_target(Examples)

function(add_stalker_example TARGET_NAME SRC_FILE)
    add_executable(${TARGET_NAME} ${SRC_FILE})
    target_link_libraries(${TARGET_NAME} PRIVATE Stalker::Stalker)
    set_target_properties(${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Examples")
    add_dependencies(Examples ${TARGET_NAME})
endfunction()

add_stalker_example(ExampleAPIShowcase APIShowcase/ExampleAPIShowcase.cpp)
add_stalker_example(ExampleTemplateMetaprogramming TemplateMetaprogramming/ExampleTemplateMetaprogramming.cpp)
