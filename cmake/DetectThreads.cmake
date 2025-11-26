# Threading backend + count negotiation
function(stalker_threads_init)
    if(STALKER_PLATFORM STREQUAL "ESP32")
        if(STALKER_THREADING_ENABLE)
            message(WARNING "Threading backends are unavailable on ESP32; disabling")
        endif()
        set(STALKER_THREADING_ENABLE OFF CACHE BOOL "Threads disabled on ESP32" FORCE)
        set(STALKER_THREADING_STD_ENABLE OFF CACHE BOOL "STD thread backend disabled" FORCE)
        set(STALKER_THREADING_POSIX_ENABLE OFF CACHE BOOL "POSIX backend disabled" FORCE)
        set(STALKER_THREADING_POSIX_SMT_ENABLE OFF CACHE BOOL "SMT disabled" FORCE)
        set(STALKER_THREADING_MAX_THREADS_ENABLE OFF CACHE BOOL "Max threads disabled" FORCE)
        set(STALKER_THREADING_NUM_THREADS 0 CACHE STRING "Threads disabled" FORCE)
        set(STALKER_THREADING_COMPILE_DEFINES STALKER_THREADING_ENABLE=0 PARENT_SCOPE)
        return()
    endif()

    if(NOT STALKER_THREADING_ENABLE)
        set(STALKER_THREADING_NUM_THREADS 0 CACHE STRING "Threads disabled" FORCE)
        set(STALKER_THREADING_COMPILE_DEFINES STALKER_THREADING_ENABLE=0 PARENT_SCOPE)
        return()
    endif()

    include(ProcessorCount)
    ProcessorCount(_num)
    if(_num EQUAL 0)
        set(_num 2)
    endif()

    # Determine effective thread count without forcing cache updates
    if(STALKER_THREADING_MAX_THREADS_ENABLE)
        set(_eff ${_num})
    else()
        # If 0 or unset, default to 2 (per user request)
        if(NOT STALKER_THREADING_NUM_THREADS OR STALKER_THREADING_NUM_THREADS EQUAL 0)
            set(_eff 2)
        else()
            set(_eff ${STALKER_THREADING_NUM_THREADS})
        endif()
    endif()

    # Clamp to hardware limit if we defaulted to 2 but have only 1 core?
    if(_eff GREATER _num AND _num GREATER 0)
        # Optional: clamp or warn. Let's just warn and keep user value if explicit, 
        # but if we defaulted to 2 and have 1 core, maybe we should respect 1?
        # User said "set to 2". Let's stick to 2 unless hardware is 1.
        if(_num EQUAL 1 AND _eff EQUAL 2)
             set(_eff 1)
        endif()
    endif()

    set(STALKER_THREADING_NUM_THREADS ${_eff} CACHE STRING "Thread count" FORCE)

    # Backend exclusivity (simplified)
    if(STALKER_THREADING_POSIX_ENABLE AND STALKER_THREADING_STD_ENABLE)
        if(STALKER_PLATFORM STREQUAL "LINUX")
            set(STALKER_THREADING_STD_ENABLE OFF CACHE BOOL "Disabled due to POSIX preference" FORCE)
        else()
            set(STALKER_THREADING_POSIX_ENABLE OFF CACHE BOOL "Disabled (non-Linux)" FORCE)
        endif()
    elseif(NOT STALKER_THREADING_POSIX_ENABLE AND NOT STALKER_THREADING_STD_ENABLE)
        if(STALKER_PLATFORM STREQUAL "LINUX")
            set(STALKER_THREADING_POSIX_ENABLE ON CACHE BOOL "Default POSIX" FORCE)
        else()
            set(STALKER_THREADING_STD_ENABLE ON CACHE BOOL "Default std::thread" FORCE)
        endif()
    endif()

    # SMT only valid with POSIX
    if(STALKER_THREADING_STD_ENABLE AND STALKER_THREADING_POSIX_SMT_ENABLE)
        set(STALKER_THREADING_POSIX_SMT_ENABLE OFF CACHE BOOL "SMT invalid with std::thread" FORCE)
    endif()

    # Link threading library (pthread or similar)
    find_package(Threads REQUIRED)
    set(STALKER_THREADING_LINK_LIBS Threads::Threads PARENT_SCOPE)

    set(defs
        STALKER_THREADING_ENABLE
        STALKER_THREADING_NUM_THREADS=${STALKER_THREADING_NUM_THREADS}
        STALKER_THREADING_POSIX_ENABLE=$<BOOL:${STALKER_THREADING_POSIX_ENABLE}>
        STALKER_THREADING_POSIX_SMT_ENABLE=$<BOOL:${STALKER_THREADING_POSIX_SMT_ENABLE}>
        STALKER_THREADING_STD_ENABLE=$<BOOL:${STALKER_THREADING_STD_ENABLE}>
    )
    set(STALKER_THREADING_COMPILE_DEFINES ${defs} PARENT_SCOPE)
endfunction()