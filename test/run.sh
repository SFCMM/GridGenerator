#!/bin/bash

BINPATH=""
SUCCESS=0
FAILED=0

if [[ -z "${SFCMM_GRIDGEN_BIN}" ]]; then
  echo "Environment variable SFCMM_GRIDGEN_BIN is not set!"
  exit
else
  BINPATH="${SFCMM_GRIDGEN_BIN}"
fi

DEFAULT_OMP_THREADS=4
LOG_PATH="run_log_"$(date +%s)

write_report() {
  touch "$LOG_PATH"/report.csv.txt
  echo "testcase, passed?, time" >>"$LOG_PATH"/report.csv.txt
}

time_test() {
  exec 3>&1 4>&2
  #  time_result=$({ time run_test "$1" 1>&3 2>&4; } 2>&1)
  time_result=$({ time run_test "$1" 1>&3 2>&4; } 2>&1)
  exec 3>&- 4>&-

  #only get user time
  usertime=$(echo -n "$time_result" | grep user)

  #only get numerical value by delimiting the space
  usertime=$(echo $usertime | cut -d' ' -f2)

  #replace , with .
  usertime=$(echo $usertime | tr , .)

  if [ -f ../"$LOG_PATH"/SUCCESS ]; then
    echo "$1" ", passed," "$usertime" >>../"$LOG_PATH"/report.csv.txt
    SUCCESS=$((SUCCESS + 1))
    rm ../"$LOG_PATH"/SUCCESS
  fi
  if [ -f ../"$LOG_PATH"/FAILED ]; then
    echo "$1" ", failed," "$usertime" >>../"$LOG_PATH"/report.csv.txt
    FAILED=$((FAILED + 1))
    rm ../"$LOG_PATH"/FAILED
  fi
}

run_test() {
  NAME="$(cut -d'.' -f1 <<<"$1")"

  echo "Running testcase $NAME..."

  if OMP_NUM_THREADS=$DEFAULT_OMP_THREADS $BINPATH "$1" &>"$NAME".stdout; then
    echo "passed"
    touch ../"$LOG_PATH"/SUCCESS
  else
    echo "FAILED"
    touch ../"$LOG_PATH"/FAILED
  fi

  if [ -f "lbm_log" ]; then
    cp lbm_log ../"$LOG_PATH"/"$NAME"_lbm_log.txt
  fi
  if [ -f "lpt_log" ]; then
    cp lpt_log ../"$LOG_PATH"/"$NAME"_lpt_log.txt
  fi
  if [ -f "gridgen_log" ]; then
    cp gridgen_log ../"$LOG_PATH"/"$NAME"_gridgen_log.txt
  fi
  cp "$NAME".stdout ../"$LOG_PATH"/"$NAME".stdout.txt
}

./clean.sh
mkdir "$LOG_PATH"
write_report

cd stl || exit
time_test cube.json
time_test donut.json
time_test donut_highres.json
time_test donut_lowres.json
time_test donut_lowres_outputlvl3.json
time_test donut_lowres_outputlvl4.json
time_test f22.json
time_test sphere.json
time_test sphere_lowres.json
time_test sphere_highres.json
time_test sphere_ultrares.json
time_test sphere_ultrares_large.json
cd ..

cd box || exit
time_test box1.json
time_test box1_aligned.json
cd ..

./clean.sh

if [ $FAILED == 0 ]; then
  echo "All $SUCCESS testcases have been successful"
  exit 0
else
  echo "$FAILED testcases have failed"
  exit 1
fi
