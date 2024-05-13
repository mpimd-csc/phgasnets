#!/bin/bash

cmake -B .build -S . -DCMAKE_BUILD_TYPE="Release"
cmake --build .build

# Run single pipe testcase
echo -e "\e[33m[DEMO] Run four_compressor_types\e[0m"
cd demos/single_pipe
time ../../.build/demos/single_pipe/single_pipe config.json
./plot -c config.json
cd ../../

# Run two pipe + compressor testcase
echo -e "\e[33m[DEMO] Run four_compressor_types\e[0m"
cd demos/two_pipe_compressor
../../.build/demos/two_pipe_compressor/two_pipe_compressor config.json
./plot -c config.json -p 0
./plot -c config.json -p 1
cd ../../

# Run four compressor types
echo -e "\e[33m[DEMO] Run four_compressor_types demo\e[0m"
cd demos/four_compressor_types
../../.build/demos/four_compressor_types/four_compressor_types config.json
./plot -c config.json -p 0
./plot -c config.json -p 1
cd ../../
