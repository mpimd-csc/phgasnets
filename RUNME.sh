#!/bin/bash

PROJECT_DIR="${PROJECT_DIR:=${PWD}}"
echo -e "\e[33mRunning from project directory:\e[0m ${PROJECT_DIR}"

BUILD_DIR="${BUILD_DIR:=build}"
echo -e "\e[33mBuild directory:\e[0m ${PROJECT_DIR}/${BUILD_DIR}"

DEMO_DIR="${PROJECT_DIR}/${BUILD_DIR}/demos"

# Run single pipe testcase
echo -e "\e[33m[DEMO] Run single_pipe_testcase\e[0m"
pushd demos/single_pipe
time ${DEMO_DIR}/single_pipe/single_pipe config.json
./plot -c config.json
popd > /dev/null

# Run two pipe + compressor testcase
echo -e "\e[33m[DEMO] Run two_pipe_compressor\e[0m"
pushd demos/two_pipe_compressor
${DEMO_DIR}/two_pipe_compressor/two_pipe_compressor config.json
./plot -c config.json -p 0
./plot -c config.json -p 1
popd > /dev/null

# Run four compressor types
echo -e "\e[33m[DEMO] Run four_compressor_types\e[0m"
pushd demos/four_compressor_types
${DEMO_DIR}/four_compressor_types/four_compressor_types config.json
./plot -c config.json -p 0
./plot -c config.json -p 1
popd > /dev/null
