#!/bin/bash

SetEnvironment () {
  echo "=================================================================="
  echo "=================STALKER ENVIRONMENT VARIABLES===================="
  echo "=================================================================="
  export STALKER_ROOTDIR=${CODE}/stalker
  export STALKER_BIN_PATH=${STALKER_ROOTDIR}/build
  export BIN_NAME="stalker"
  export CMAKE_PATH=/usr/bin/cmake
  export TESTS_PATH=${STALKER_ROOTDIR}/Tests
  export LOGS_PATH=${TESTS_PATH}/STLKR_PerformanceTests/StalkerVector/logs/Copy
  export PYTHON_LOG_ANALYZER=${STALKER_ROOTDIR}/Utility/LogAnalyzer.py

  echo "STALKER_ROOTDIR:               $STALKER_ROOTDIR"
  echo "STALKER_BIN_PATH:              $STALKER_BIN_PATH"
  echo "BIN_NAME:                      $BIN_NAME"
  echo "CMAKE_PATH:                    $CMAKE_PATH"
  echo "TESTS_PATH:                    $TESTS_PATH"
  echo "LOGS_PATH:                     $LOGS_PATH"
  echo "PYTHON_LOG_ANALYZER:           $PYTHON_LOG_ANALYZER"
  #echo "CMAKE_PATH:                    $CMAKE_PATH"
}

BuildStalker () {
  SetEnvironment
  echo "Building stalker..."
  $CMAKE_PATH --build "$STALKER_BIN_PATH" --target $BIN_NAME -j 10
  echo "Finished!"
  echo ""
}
RunStalker () {
  buildstlkr
  echo "Running stalker..."
  "$STALKER_BIN_PATH"/${BIN_NAME}
  echo "Finished!"
}
CleanStalker () {
    SetEnvironment
    echo "Cleaning stalker build..."
    cd "$STALKER_ROOT"
    rm -rf build
    echo "Finished cleaning!"
    echo ""
}
AnalyzeLogs () {
  python3 "$PYTHON_LOG_ANALYZER" "$LOGS_PATH"
}

ValgrindStalker () {
  # Define the log file path
  LOG_FILE=${TESTS_PATH}/ValgrindOutputs/valgrind_$(date '+%d%m%Y_%H%M%S').log

  # Create the directory if it does not exist
  mkdir -p "$(dirname "$LOG_FILE")"

  # Initialize the Valgrind arguments
  VALGRIND_ARGS=""
  VALGRIND_ARGS+=" --tool=memcheck"
  VALGRIND_ARGS+=" --leak-check=full"
  VALGRIND_ARGS+=" --leak-resolution=high"
  VALGRIND_ARGS+=" --track-origins=yes"
  VALGRIND_ARGS+=" --errors-for-leak-kinds=all"
  VALGRIND_ARGS+=" --error-limit=no"
  VALGRIND_ARGS+=" --num-callers=50"
  VALGRIND_ARGS+=" --show-reachable=yes"
  VALGRIND_ARGS+=" --show-possibly-lost=yes"
  VALGRIND_ARGS+=" --undef-value-errors=yes"
  VALGRIND_ARGS+=" --log-file=$LOG_FILE"

  echo "Running Valgrind with args: $VALGRIND_ARGS"
  valgrind $VALGRIND_ARGS "$STALKER_BIN_PATH/$BIN_NAME"
  echo "Valgrind log saved to $LOG_FILE"
}

HelgrindStalker () {
  # Define the log file path
  LOG_FILE=${TESTS_PATH}/ValgrindOutputs/valgrind_helgrind_$(date '+%d%m%Y_%H%M%S').log

  # Create the directory if it does not exist
  mkdir -p "$(dirname "$LOG_FILE")"

  # Initialize the Valgrind arguments
  VALGRIND_ARGS=""
  VALGRIND_ARGS+=" --tool=helgrind"
  VALGRIND_ARGS+=" --log-file=$LOG_FILE"
  VALGRIND_ARGS+=" --history-level=full"       # Full history for race condition detection
  VALGRIND_ARGS+=" --fair-sched=yes"           # Use fair scheduling

  echo "Running Valgrind with args: $VALGRIND_ARGS"
  valgrind $VALGRIND_ARGS "$STALKER_BIN_PATH/$BIN_NAME"
  echo "Valgrind log saved to $LOG_FILE"
}

alias setstlkr=SetEnvironment

alias helgrindstlkr=HelgrindStalker

alias buildstlkr=BuildStalker

alias runstlkr=RunStalker

alias cleanstlkr=CleanStalker

alias analyzestlkr=AnalyzeLogs

alias valgrindstlkr=ValgrindStalker
