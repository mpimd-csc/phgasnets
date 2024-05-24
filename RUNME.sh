#!/bin/bash

PROJECT_DIR="${PROJECT_DIR:=${PWD}}"
echo -e "\e[33m[INFO] Project directory:\e[0m ${PROJECT_DIR}"

BUILD_DIR="${BUILD_DIR:=${PROJECT_DIR}/build}"
echo -e "\e[33m[INFO] Build directory:\e[0m ${BUILD_DIR}"

OUT_DIR="${OUT_DIR:=.}"
echo -e "\e[33m[INFO] Results directory:\e[0m ${OUT_DIR}"

DEMO_DIR="${BUILD_DIR}/demos"

# Run single pipe testcase
echo -e "\e[33m[DEMO] Run single_pipe_testcase\e[0m"
pushd ${PROJECT_DIR}/demos/single_pipe > /dev/null
${DEMO_DIR}/single_pipe/single_pipe config.json
./plot -c config.json -O ${OUT_DIR}
popd > /dev/null

# Run two pipe + compressor testcase
echo -e "\e[33m[DEMO] Run two_pipe_compressor\e[0m"
pushd ${PROJECT_DIR}/demos/two_pipe_compressor > /dev/null
${DEMO_DIR}/two_pipe_compressor/two_pipe_compressor config.json
./plot -c config.json -p 0 -O ${OUT_DIR}
./plot -c config.json -p 1 -O ${OUT_DIR}
popd > /dev/null

# Run four compressor types
echo -e "\e[33m[DEMO] Run four_compressor_types\e[0m"
compressor_types=("fcam" "fcav" "fpav" "fpam")
pushd ${PROJECT_DIR}/demos/four_compressor_types > /dev/null
for COMPR_TYPE in "${compressor_types[@]}"
do
  CONFIG_FILE="config_${COMPR_TYPE}.json"
  ${DEMO_DIR}/four_compressor_types/four_compressor_types ${CONFIG_FILE}
  ./plot -c ${CONFIG_FILE} -p 0 -O ${OUT_DIR} --csv
  ./plot -c ${CONFIG_FILE} -p 1 -O ${OUT_DIR} --csv
done
popd > /dev/null
