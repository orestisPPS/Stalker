# SIMD instruction set negotiation (compile flags now handled centrally in ConfigureCompilerFlags)
function(stalker_simd_init)
    set(_isEsp32 FALSE)
    if(STALKER_PLATFORM STREQUAL "ESP32")
        set(_isEsp32 TRUE)
    endif()

    if(_isEsp32)
        set(_esp32_s3 FALSE)
        if(DEFINED IDF_TARGET AND IDF_TARGET STREQUAL "esp32s3")
            set(_esp32_s3 TRUE)
        elseif(DEFINED ENV{IDF_TARGET} AND "$ENV{IDF_TARGET}" STREQUAL "esp32s3")
            set(_esp32_s3 TRUE)
        elseif(DEFINED CONFIG_IDF_TARGET_ESP32S3 AND CONFIG_IDF_TARGET_ESP32S3)
            set(_esp32_s3 TRUE)
        endif()

        if(NOT _esp32_s3)
            if(STALKER_SIMD_ENABLE)
                message(WARNING "SIMD is only available on ESP32-S3; disabling for current target")
            endif()
            set(STALKER_SIMD_ENABLE OFF CACHE BOOL "SIMD disabled (non S3 ESP32)" FORCE)
            set(STALKER_SIMD_INSTRUCTION_SET "none" CACHE STRING "SIMD disabled" FORCE)
            set(STALKER_SIMD_AVX2_OK OFF CACHE BOOL "AVX2 supported" FORCE)
            set(STALKER_SIMD_AVX512_OK OFF CACHE BOOL "AVX512 supported" FORCE)
            set(STALKER_SIMD_COMPILE_DEFINES STALKER_SIMD_INSTRUCTION_SET_NONE PARENT_SCOPE)
            return()
        endif()
    endif()

    # Early exit when SIMD globally disabled.
    if(NOT STALKER_SIMD_ENABLE)
        set(STALKER_SIMD_INSTRUCTION_SET "none" CACHE STRING "SIMD disabled" FORCE)
        set(STALKER_SIMD_AVX2_OK OFF CACHE BOOL "AVX2 supported (compiler+intrinsic)" FORCE)
        set(STALKER_SIMD_AVX512_OK OFF CACHE BOOL "AVX512 supported (compiler+intrinsic)" FORCE)
        set(STALKER_SIMD_COMPILE_DEFINES STALKER_SIMD_INSTRUCTION_SET_NONE PARENT_SCOPE)
        return()
    endif()

    set(_req ${STALKER_SIMD_INSTRUCTION_SET})
    if(_isEsp32)
        # Only ESP32-S3 reaches here.
        if(NOT _req STREQUAL "esp-dsp")
            message(WARNING "ESP32-S3 builds only support esp-dsp SIMD backend; overriding request '${_req}'")
        endif()
        set(STALKER_SIMD_INSTRUCTION_SET "esp-dsp" CACHE STRING "ESP-DSP backend" FORCE)
        set(STALKER_SIMD_AVX2_OK OFF CACHE BOOL "AVX2 supported (compiler+intrinsic)" FORCE)
        set(STALKER_SIMD_AVX512_OK OFF CACHE BOOL "AVX512 supported (compiler+intrinsic)" FORCE)
        if(STALKER_SIMD_STORE_POLICY STREQUAL "stream")
            message(WARNING "Streaming store policy is not supported on ESP32-S3; switching to cache")
            set(STALKER_SIMD_STORE_POLICY "cache" CACHE STRING "SIMD store policy" FORCE)
        endif()
        set(defs STALKER_SIMD_INSTRUCTION_SET_ESP_DSP STALKER_SIMD_STORE_CACHE)
        set(STALKER_SIMD_COMPILE_DEFINES ${defs} PARENT_SCOPE)
        return()
    endif()

    # Probe capabilities (compiler support). Use compile checks to be cross-compile friendly.
    enable_language(C)
    include(CheckCSourceCompiles)

    # AVX2 probe
    set(CMAKE_REQUIRED_FLAGS "-mavx2")
    check_c_source_compiles("#include <immintrin.h>\nint main(){__m256i x=_mm256_set1_epi32(0);(void)x;return 0;}" _HAS_AVX2)

    # AVX512 probe (foundation flag set)
    set(CMAKE_REQUIRED_FLAGS "-mavx512f")
    check_c_source_compiles("#include <immintrin.h>\nint main(){__m512i x=_mm512_set1_epi32(0);(void)x;return 0;}" _HAS_AVX512F)
    unset(CMAKE_REQUIRED_FLAGS)

    set(STALKER_SIMD_AVX2_OK ${_HAS_AVX2} CACHE BOOL "AVX2 supported (compiler+intrinsic)" FORCE)
    set(STALKER_SIMD_AVX512_OK ${_HAS_AVX512F} CACHE BOOL "AVX512 supported (compiler+intrinsic)" FORCE)

    # Decide active/default instruction set.
    # Rules:
    #  - auto: prefer AVX2 if available else none (do NOT select AVX512 automatically)
    #  - avx512: choose avx512 if supported, else downgrade to avx2 if available else none
    #  - avx2: choose avx2 if supported else none
    #  - none: scalar
    #  - unknown: warn and treat as none
    if(_req STREQUAL "auto")
        if(STALKER_SIMD_AVX2_OK)
            set(STALKER_SIMD_INSTRUCTION_SET "avx2" CACHE STRING "Auto-selected (pref AVX2)" FORCE)
        else()
            set(STALKER_SIMD_INSTRUCTION_SET "none" CACHE STRING "Auto-selected (no SIMD)" FORCE)
        endif()
    elseif(_req STREQUAL "avx512")
        if(STALKER_SIMD_AVX512_OK)
            set(STALKER_SIMD_INSTRUCTION_SET "avx512" CACHE STRING "Requested" FORCE)
        elseif(STALKER_SIMD_AVX2_OK)
            message(WARNING "Requested AVX512 not supported; downgrading to AVX2")
            set(STALKER_SIMD_INSTRUCTION_SET "avx2" CACHE STRING "Downgraded" FORCE)
        else()
            message(WARNING "Requested AVX512 not supported; disabling SIMD")
            set(STALKER_SIMD_INSTRUCTION_SET "none" CACHE STRING "Unavailable" FORCE)
        endif()
    elseif(_req STREQUAL "avx2")
        if(STALKER_SIMD_AVX2_OK)
            set(STALKER_SIMD_INSTRUCTION_SET "avx2" CACHE STRING "Requested" FORCE)
        else()
            message(WARNING "Requested AVX2 not supported; disabling SIMD")
            set(STALKER_SIMD_INSTRUCTION_SET "none" CACHE STRING "Unavailable" FORCE)
        endif()
    elseif(_req STREQUAL "none")
        set(STALKER_SIMD_INSTRUCTION_SET "none" CACHE STRING "Requested" FORCE)
    else()
        message(WARNING "Unknown SIMD instruction set '${_req}' → disabling")
        set(STALKER_SIMD_INSTRUCTION_SET "none" CACHE STRING "Unknown" FORCE)
    endif()

    # Disable SIMD globally if active instruction set is none.
    if(STALKER_SIMD_INSTRUCTION_SET STREQUAL "none")
        set(STALKER_SIMD_ENABLE OFF CACHE BOOL "SIMD disabled" FORCE)
    endif()

    # Store policy validation (only meaningful when enabled)
    if(STALKER_SIMD_ENABLE)
        if(NOT STALKER_SIMD_STORE_POLICY MATCHES "^(stream|cache)$")
            message(FATAL_ERROR "Invalid STALKER_SIMD_STORE_POLICY=${STALKER_SIMD_STORE_POLICY}")
        endif()
    endif()

    # Build compile definitions only (ISA compile options appended in stalker_compiler_flags_init).
    # Capability macros: always reflect what was probed (even if user selected none), but we only
    # emit them when SIMD still enabled to avoid accidental inclusion in scalar-only builds.
    set(defs)

    if(STALKER_SIMD_ENABLE)
        # Capability macros
        if(STALKER_SIMD_AVX2_OK)
            list(APPEND defs STALKER_SIMD_AVX2_OK)
        endif()
        if(STALKER_SIMD_AVX512_OK)
            list(APPEND defs STALKER_SIMD_AVX512_OK)
        endif()

        # Instruction set macros: if both capabilities exist, emit both so code can compile both specializations.
        if(STALKER_SIMD_AVX512_OK AND STALKER_SIMD_AVX2_OK)
            list(APPEND defs STALKER_SIMD_INSTRUCTION_SET_AVX512 STALKER_SIMD_INSTRUCTION_SET_AVX2)
        elseif(STALKER_SIMD_AVX512_OK)
            list(APPEND defs STALKER_SIMD_INSTRUCTION_SET_AVX512)
        elseif(STALKER_SIMD_AVX2_OK)
            list(APPEND defs STALKER_SIMD_INSTRUCTION_SET_AVX2)
        else()
            list(APPEND defs STALKER_SIMD_INSTRUCTION_SET_NONE)
        endif()

        # Store policy macro
        if(STALKER_SIMD_STORE_POLICY STREQUAL "stream")
            list(APPEND defs STALKER_SIMD_STORE_STREAM)
        else()
            list(APPEND defs STALKER_SIMD_STORE_CACHE)
        endif()
    else()
        list(APPEND defs STALKER_SIMD_INSTRUCTION_SET_NONE)
        # Capabilities intentionally not exported when disabled.
    endif()

    set(STALKER_SIMD_COMPILE_DEFINES ${defs} PARENT_SCOPE)
endfunction()