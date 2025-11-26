# ESP32Integration.cmake
# Helpers to bridge Stalker into ESP-IDF component builds while keeping host builds untouched.

macro(stalker_esp32_component_init)
    message(STATUS "Checking ESP32 integration: STALKER_PLATFORM=${STALKER_PLATFORM}")

    set(STALKER_ESP32_COMPONENT_ACTIVE FALSE)

    if(STALKER_PLATFORM STREQUAL "ESP32")
        if(NOT COMMAND idf_component_register)
            message(STATUS "ESP32 platform detected without ESP-IDF component context; skipping idf_component_register integration")
        else()
            message(STATUS "idf_component_register found, registering component")

            # Header-only component; rely on the INTERFACE library for logic but expose includes to IDF.
            idf_component_register(
                SRCS ""
                INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include"
                REQUIRES
            )

            if(DEFINED COMPONENT_LIB AND TARGET ${COMPONENT_LIB})
                target_link_libraries(${COMPONENT_LIB} INTERFACE Stalker)
            else()
                message(WARNING "ESP-IDF component library target not found; unable to link Stalker automatically")
            endif()

            set(STALKER_ESP32_COMPONENT_ACTIVE TRUE)
        endif()
    else()
        message(STATUS "ESP32 integration skipped (platform=${STALKER_PLATFORM})")
    endif()
endmacro()
