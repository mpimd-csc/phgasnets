#!/bin/bash
BUILD_DIR=.build/

# Run single pipe testcase
echo -e "\e[33m[DEMO] Run single_pipe_testcase\e[0m"
pushd demos/single_pipe
time ../../${BUILD_DIR}/demos/single_pipe/single_pipe config.json
./plot -c config.json
popd

# Run two pipe + compressor testcase
echo -e "\e[33m[DEMO] Run two_pipe_compressor\e[0m"
pushd demos/two_pipe_compressor
../../${BUILD_DIR}/demos/two_pipe_compressor/two_pipe_compressor config.json
./plot -c config.json -p 0
./plot -c config.json -p 1
popd

# Run four compressor types
echo -e "\e[33m[DEMO] Run four_compressor_types\e[0m"
pushd demos/four_compressor_types
../../${BUILD_DIR}/demos/four_compressor_types/four_compressor_types config.json
./plot -c config.json -p 0
./plot -c config.json -p 1
popd
