#!/bin/bash

PHGASNETS_PROJECT_DIR="${PHGASNETS_PROJECT_DIR:=$(dirname "$(realpath "$0")")}"
echo -e "\e[1;33m[INFO] Project directory:\e[0m ${PHGASNETS_PROJECT_DIR}"

PHGASNETS_BUILD_DIR="${PHGASNETS_BUILD_DIR:=${PHGASNETS_PROJECT_DIR}/build}"
echo -e "\e[1;33m[INFO] Build directory:\e[0m ${PHGASNETS_BUILD_DIR}"

PHGASNETS_OUT_DIR="${PHGASNETS_OUT_DIR:=${PHGASNETS_PROJECT_DIR}/results}"
echo -e "\e[1;33m[INFO] Results directory:\e[0m ${PHGASNETS_OUT_DIR}"

PHGASNETS_DEMO_DIR="${PHGASNETS_BUILD_DIR}/demos"

# Quit on error
set -e

# Restrict threading if you use the parallel BLAS version
# https://wiki.debian.org/DebianScience/LinearAlgebraLibraries
export OMP_NUM_THREADS=1

# Change to output directory
# Create the directory if it doesn't exist
[ -d "$PHGASNETS_OUT_DIR" ] || mkdir -p "$PHGASNETS_OUT_DIR"
pushd ${PHGASNETS_OUT_DIR} > /dev/null

# Run single pipe testcase
echo -e "\e[1;33m[DEMO] Run single_pipe_testcase\e[0m"
PHGASNETS_CONFIG_FILE=${PHGASNETS_PROJECT_DIR}/demos/single_pipe/config.json
${PHGASNETS_DEMO_DIR}/single_pipe/single_pipe -c ${PHGASNETS_CONFIG_FILE} --csv
${PHGASNETS_PROJECT_DIR}/demos/single_pipe/plot -c ${PHGASNETS_CONFIG_FILE} -O ${PHGASNETS_OUT_DIR}

# Run four compressor types
echo -e "\e[1;33m[DEMO] Run four_compressor_types\e[0m"
compressor_types=("fcam" "fcav" "fpav" "fpam" "nocompressor")
for COMPR_TYPE in "${compressor_types[@]}"
do
  echo -e "\e[33m[COMPR_TYPE] ${COMPR_TYPE}\e[0m"
  PHGASNETS_CONFIG_FILE=${PHGASNETS_PROJECT_DIR}/demos/four_compressor_types/"config_${COMPR_TYPE}.json"
  ${PHGASNETS_DEMO_DIR}/four_compressor_types/four_compressor_types ${PHGASNETS_CONFIG_FILE}
  ${PHGASNETS_PROJECT_DIR}/demos/four_compressor_types/plot -c ${PHGASNETS_CONFIG_FILE} -p 0 -O ${PHGASNETS_OUT_DIR} --csv
  ${PHGASNETS_PROJECT_DIR}/demos/four_compressor_types/plot -c ${PHGASNETS_CONFIG_FILE} -p 1 -O ${PHGASNETS_OUT_DIR} --csv
done
${PHGASNETS_PROJECT_DIR}/demos/four_compressor_types/plotall -O ${PHGASNETS_OUT_DIR} --compressor-types "${compressor_types[@]}"

# Change back to start directory
popd > /dev/null
