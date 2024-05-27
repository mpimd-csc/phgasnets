#!/bin/bash

PROJECT_DIR="${PROJECT_DIR:=$(dirname "$0")}"
echo -e "\e[33m[INFO] Project directory:\e[0m ${PROJECT_DIR}"

BUILD_DIR="${BUILD_DIR:=${PROJECT_DIR}/build}"
echo -e "\e[33m[INFO] Build directory:\e[0m ${BUILD_DIR}"

OUT_DIR="${OUT_DIR:=${PROJECT_DIR}/results}"
echo -e "\e[33m[INFO] Results directory:\e[0m ${OUT_DIR}"

DEMO_DIR="${BUILD_DIR}/demos"

# Change to output directory
pushd ${OUT_DIR} > /dev/null

# Run single pipe testcase
echo -e "\e[33m[DEMO] Run single_pipe_testcase\e[0m"
CONFIG_FILE=${PROJECT_DIR}/demos/single_pipe/config.json
${DEMO_DIR}/single_pipe/single_pipe ${CONFIG_FILE}
${PROJECT_DIR}/demos/single_pipe/plot -c ${CONFIG_FILE} -O ${OUT_DIR}

# Run two pipe + compressor testcase
echo -e "\e[33m[DEMO] Run two_pipe_compressor\e[0m"
CONFIG_FILE=${PROJECT_DIR}/demos/two_pipe_compressor/config.json
${DEMO_DIR}/two_pipe_compressor/two_pipe_compressor ${CONFIG_FILE}
${PROJECT_DIR}/demos/two_pipe_compressor/plot -c ${CONFIG_FILE} -p 0 -O ${OUT_DIR}
${PROJECT_DIR}/demos/two_pipe_compressor/plot -c ${CONFIG_FILE} -p 1 -O ${OUT_DIR}

# Run four compressor types
echo -e "\e[33m[DEMO] Run four_compressor_types\e[0m"
compressor_types=("fcam" "fcav" "fpav" "fpam")
for COMPR_TYPE in "${compressor_types[@]}"
do
  CONFIG_FILE=${PROJECT_DIR}/demos/four_compressor_types/"config_${COMPR_TYPE}.json"
  ${DEMO_DIR}/four_compressor_types/four_compressor_types ${CONFIG_FILE}
  ${PROJECT_DIR}/demos/four_compressor_types/plot -c ${CONFIG_FILE} -p 0 -O ${OUT_DIR} --csv
  ${PROJECT_DIR}/demos/four_compressor_types/plot -c ${CONFIG_FILE} -p 1 -O ${OUT_DIR} --csv
done
${PROJECT_DIR}/demos/four_compressor_types/plotall -O ${OUT_DIR}

# Change back to start directory
popd > /dev/null