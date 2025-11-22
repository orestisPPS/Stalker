# Alignment configuration separated from platform detection.
function(stalker_alignment_init)
    # Empty or undefined value signals auto-default to platform cache line size.
    if(NOT DEFINED STALKER_ALIGNMENT OR STALKER_ALIGNMENT STREQUAL "")
        set(STALKER_ALIGNMENT ${STALKER_PLATFORM_CACHE_LINE_SIZE} CACHE STRING "Data alignment in bytes" FORCE)
        message(STATUS "STALKER_ALIGNMENT auto-set to cache line size = ${STALKER_ALIGNMENT} bytes")
    endif()
    math(EXPR _align_minus_one "${STALKER_ALIGNMENT} - 1")
    math(EXPR _align_and "${STALKER_ALIGNMENT} & ${_align_minus_one}")
    if(NOT STALKER_ALIGNMENT GREATER 0 OR NOT _align_and EQUAL 0)
        message(FATAL_ERROR "STALKER_ALIGNMENT (${STALKER_ALIGNMENT}) must be positive power of 2")
    endif()
endfunction()