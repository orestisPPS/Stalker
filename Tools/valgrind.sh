#!/bin/bash

# Simplified Valgrind memcheck runner (helgrind removed).
# Usage:
#   bash Tools/valgrind.sh -exec /path/to/binary [-out /path/to/output_dir]
# Example:
#   bash Tools/valgrind.sh -exec build/Tests/Tests -out ./valgrind-logs

set -euo pipefail

usage() {
	cat <<USAGE
Valgrind memcheck runner
	-exec <path>   Executable to analyze (required)
	-out  <dir>    Output directory for log (optional; default ./valgrind-logs)
	-h|--help      Show this help
USAGE
}

valgrind_memcheck() {
	local exec_path=""
	local out_dir="./valgrind-logs"

	while [[ $# -gt 0 ]]; do
		case "$1" in
			-exec) shift; exec_path="${1:-}" ;;
			-out)  shift; out_dir="${1:-}" ;;
			-h|--help) usage; return 0 ;;
			*) echo "[ERROR] Unknown arg: $1" >&2; usage; return 2 ;;
		esac
		shift || true
	done

	if [[ -z "$exec_path" ]]; then
		echo "[ERROR] -exec is required" >&2; usage; return 2
	fi
	if [[ ! -f "$exec_path" ]]; then
		echo "[ERROR] Executable not found: $exec_path" >&2; return 3
	fi
	if [[ ! -x "$exec_path" ]]; then
		echo "[ERROR] Executable not runnable (chmod +x?): $exec_path" >&2; return 4
	fi
	if ! command -v valgrind >/dev/null 2>&1; then
		echo "[ERROR] valgrind not found in PATH" >&2; return 5
	fi

	mkdir -p "$out_dir"
	local ts
	ts=$(date '+%Y%m%d_%H%M%S')
	local LOG_FILE="${out_dir}/valgrind_${ts}.log"

		VALGRIND_ARGS=(
			--tool=memcheck
			--leak-check=full
			--leak-resolution=high
			--track-origins=yes
			--errors-for-leak-kinds=all
			--error-limit=no
			--num-callers=50
			--show-reachable=yes
			--show-possibly-lost=yes
			--undef-value-errors=yes
			--log-file="${LOG_FILE}"
		)

		echo "Running Valgrind with args:"
		for a in "${VALGRIND_ARGS[@]}"; do echo "  $a"; done

		set +e
		valgrind "${VALGRIND_ARGS[@]}" "$exec_path"
		local status=$?
		set -e
		if [[ $status -ne 0 ]]; then
			if grep -q "Unrecognised instruction" "$LOG_FILE" && grep -q "62 D1 FE" "$LOG_FILE"; then
				echo "[HINT] Valgrind may not support AVX512 instructions on this build. Reconfigure with: -DSTALKER_SIMD_INSTRUCTION_SET=avx2 or disable SIMD." >&2
			fi
			echo "[ERROR] Valgrind execution failed (log: $LOG_FILE)" >&2
			return 6
		fi
		echo "Valgrind log saved to $LOG_FILE"
}

if [[ "${BASH_SOURCE[0]}" == "$0" ]]; then
	valgrind_memcheck "$@"
fi

