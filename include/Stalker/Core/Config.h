/*
 * Copyright 2024-2026 Papas Christoforos Orestis
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @file Config.h
 * @brief Central configuration header for the Stalker project.
 *
 * This header includes various configuration files that set up compiler-specific
 * settings, SIMD support, CUDA integration, OpenMP usage, parallelization options,
 * memory alignment, symbol visibility, and loop unrolling optimizations.
 *
 * Included configuration headers:
 * - Config/Compiler.h: Compiler-specific macros and settings.
 * - Config/SIMD.h: SIMD (Single Instruction, Multiple Data) support.[Conditionally if SIMD is supported and enabled]
 * - Config/Parallel.h: General parallelization options. [Conditionally if threading is enabled]
 * - Config/Alignment.h: Memory alignment macros and utilities.
 * - Config/LoopUnrolling.h: Loop unrolling optimization macros.
 *
 * @note This file is intended to be included in most source files to ensure
 *       consistent configuration across the project.
 */
#pragma once

#include "Config/Platform.h"
#include "Config/Alignment.h"
#include "Config/Compiler.h"
#include "Config/LoopUnrolling.h"
#if defined(STALKER_THREADING_ENABLE) && STALKER_THREADING_ENABLE != 0
#include "Config/Parallel.h"
#endif
#if defined(STALKER_SIMD_ENABLE) && STALKER_SIMD_ENABLE != 0 && \
   (defined(STALKER_SIMD_AVX2_OK) || defined(STALKER_SIMD_AVX512_OK))
#include "Config/SIMD.h"
#endif