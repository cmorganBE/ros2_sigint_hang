#!/bin/bash
set -uo pipefail

EXECUTABLE="sigint-hang-cpp"

# return 0 upon success, non-zero upon failure
function test_run() {
  ros2 run ros2_sigint_hang ${EXECUTABLE} &
  PID=$!

  # wait for a little bit so the process can start
  SLEEP_SECONDS=10
  echo "Sleeping for ${SLEEP_SECONDS} seconds for the process to start"
  sleep ${SLEEP_SECONDS}

  echo "sending -INT to all ${EXECUTABLE}"
  killall -INT ${EXECUTABLE}

  SLEEP_SHUTDOWN_SECONDS=2
  echo "Sleeping for ${SLEEP_SHUTDOWN_SECONDS} seconds for shutdown"
  sleep ${SLEEP_SHUTDOWN_SECONDS}

  # did the process shut down?
  PGREP_OUTPUT=$(pgrep -x ${EXECUTABLE})
  WAS_RUNNING=$?
  if [ ${WAS_RUNNING} -eq 0 ]
  then
      echo "Running"
      RETVAL=1
  else
      echo "Stopped"
      RETVAL=0
  fi

  return ${RETVAL}
}

echo "Starting test run"
test_run
TEST_RUN_RESULT=$?
if [ ${TEST_RUN_RESULT} -eq 0 ]
then
  echo "test_run passed"
else
  echo "test_run failed, ${EXECUTABLE} is still running after sigint"
fi
