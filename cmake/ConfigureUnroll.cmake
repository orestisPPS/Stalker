# ConfigureUnroll.cmake
# Handles validation / defaulting of STALKER_UNROLL_FACTOR

function(stalker_unroll_init)
    # If unset (empty), default to 1
    if(NOT DEFINED STALKER_UNROLL_FACTOR OR STALKER_UNROLL_FACTOR STREQUAL "")
        set(STALKER_UNROLL_FACTOR 1 CACHE STRING "Loop unroll factor for meta/classic execution." FORCE)
    endif()
    # Ensure numeric and positive
    if(NOT STALKER_UNROLL_FACTOR MATCHES "^[0-9]+$")
        message(FATAL_ERROR "STALKER_UNROLL_FACTOR must be an integer (got '${STALKER_UNROLL_FACTOR}')")
    endif()
    if(NOT STALKER_UNROLL_FACTOR GREATER 0)
        message(FATAL_ERROR "STALKER_UNROLL_FACTOR must be a positive integer (got ${STALKER_UNROLL_FACTOR})")
    endif()
endfunction()