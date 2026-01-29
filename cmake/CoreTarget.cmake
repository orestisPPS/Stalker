# CoreTarget.cmake
# Defines helper to configure the StalkerCore INTERFACE target.
# Assumes detection/configure functions have already populated cache vars

function(stalker_core_target_init tgt)
    if(NOT TARGET ${tgt})
        message(FATAL_ERROR "stalker_core_target_init: target '${tgt}' does not exist")
    endif()

    # Apply platform + bitness + cache line size macros
    stalker_platform_apply_defines(${tgt})

    # SIMD + threading + unroll + alignment + store policy etc.
    target_compile_definitions(${tgt} INTERFACE
        STALKER_ALIGNMENT=${STALKER_ALIGNMENT}
        ${STALKER_SIMD_COMPILE_DEFINES}
        STALKER_SIMD_PREFETCH_LINES=${STALKER_SIMD_PREFETCH_LINES}
        STALKER_SIMD_PREFETCH_HINT=${STALKER_SIMD_PREFETCH_HINT}
        STALKER_SIMD_ENABLE=$<BOOL:${STALKER_SIMD_ENABLE}>
        STALKER_UNROLL_FACTOR=${STALKER_UNROLL_FACTOR}
        STALKER_THREADING_ENABLE=$<BOOL:${STALKER_THREADING_ENABLE}>
        STALKER_THREADING_NUM_THREADS=${STALKER_THREADING_NUM_THREADS}
        STALKER_THREADING_POSIX_ENABLE=$<BOOL:${STALKER_THREADING_POSIX_ENABLE}>
        STALKER_THREADING_POSIX_SMT_ENABLE=$<BOOL:${STALKER_THREADING_POSIX_SMT_ENABLE}>
        STALKER_THREADING_STD_ENABLE=$<BOOL:${STALKER_THREADING_STD_ENABLE}>
    )

    # Compile options: optimization bundle (+ ISA flags appended in compiler flags init)
    target_compile_options(${tgt} INTERFACE ${_stalkerCompilerFlags})

    if(STALKER_THREADING_LINK_LIBS)
        target_link_libraries(${tgt} INTERFACE ${STALKER_THREADING_LINK_LIBS})
    endif()

    # Include dirs
    target_include_directories(${tgt} INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    )

    # Install headers + target export (idempotent if re-run)
    install(
        DIRECTORY ${PROJECT_SOURCE_DIR}/include/Stalker/Core
        DESTINATION include/Stalker
    )
    install(TARGETS ${tgt}
        EXPORT StalkerTargets
        INCLUDES DESTINATION include
    )
endfunction()
