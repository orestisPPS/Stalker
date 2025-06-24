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
 * - Config/SIMD.h: SIMD (Single Instruction, Multiple Data) support.
 * - Config/CUDA.h: CUDA (Compute Unified Device Architecture) integration.
 * - Config/Parallel.h: General parallelization options.
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
#include "Config/CUDA.h"
#include "Config/LoopUnrolling.h"
#include "Config/Parallel.h"
#include "Config/SIMD.h"