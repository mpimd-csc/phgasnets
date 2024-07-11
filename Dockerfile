# Build a docker image manually from this file with,
#
# docker build . --tag phgasnets
#
# To run this container and save results in the host, run the following commands
#
# mkdir results
# docker run --rm -it -u $(id -u):$(id -g) -v ${PWD}/results:/phgasnets/run phgasnets
#
FROM debian:12-slim

# install all C++ dependencies through apt
RUN apt-get update && \
    # set timezone within container
    ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends tzdata && \
    # install C++ dependencies from apt
    apt-get install -y build-essential cmake git pkg-config \
        libopenblas-dev libeigen3-dev libmetis-dev libceres-dev nlohmann-json3-dev libhdf5-dev

# install highfive - not available in apt
ARG HIGHFIVE_PATH=/opt/highfive
ARG HIGHFIVE_TAG="v2.9.0"
ARG HIGHFIVE_GIT="https://github.com/BlueBrain/HighFive.git"

RUN mkdir -p ${HIGHFIVE_PATH}-src && cd ${HIGHFIVE_PATH}-src && \
    git init && git remote add origin ${HIGHFIVE_GIT} && \
    git fetch --depth 1 origin tag ${HIGHFIVE_TAG} && \
    git checkout --quiet ${HIGHFIVE_TAG} && \
    cmake -B build -S . \
        -DHIGHFIVE_USE_EIGEN=On \
        -DCMAKE_INSTALL_PREFIX=${HIGHFIVE_PATH} \
        -DHIGHFIVE_USE_BOOST=Off \
        -DHIGHFIVE_EXAMPLES=Off \
        -DHIGHFIVE_UNIT_TESTS=Off \
        -DHIGHFIVE_BUILD_DOCS=Off && \
    cmake --build build --parallel && \
    cmake --install build && cd && \
    rm -rf ${HIGHFIVE_PATH}-src

# install python dependencies for plot
RUN apt-get install -y python3-minimal python3-numpy python3-matplotlib python3-h5py && \
    # install latin modern fonts for plot
    apt-get install -y fonts-lmodern fontconfig && \
    mkdir -p /usr/local/share/fonts/otf && cd /usr/local/share/fonts/otf && \
    ln -s /usr/share/texmf/fonts/opentype/public/lm-math/latinmodern-math.otf && fc-cache

# provide a non-root user for devcontainer
# the args for UID and GID will be overridden by VSCode
# Ref https://code.visualstudio.com/remote/advancedcontainers/add-nonroot-user
ARG USERNAME=vscode
ARG USER_UID=1000
ARG USER_GID=${USER_UID}

RUN addgroup --gid ${USER_GID} ${USERNAME} && \
    adduser --disabled-password --gecos '' --uid ${USER_UID} --gid ${USER_GID} ${USERNAME} && \
    usermod --uid ${USER_UID} --gid ${USER_GID} ${USERNAME}

# paths for hosting the project files
ARG PROJECT_DIR=/phgasnets/src
ARG BUILD_DIR=/phgasnets/build
ARG OUT_DIR=/phgasnets/run

# copy relevant project source files into container
COPY demos ${PROJECT_DIR}/demos
COPY include ${PROJECT_DIR}/include
COPY src ${PROJECT_DIR}/src
COPY CMakeLists.txt README.md LICENSE.md RUNME.sh ${PROJECT_DIR}/

# build source
RUN cmake -B ${BUILD_DIR} -S ${PROJECT_DIR} -DCMAKE_BUILD_TYPE=Release && \
    cmake --build ${BUILD_DIR} --parallel

# set env variables for RUNME
ENV PROJECT_DIR=${PROJECT_DIR}
ENV BUILD_DIR=${BUILD_DIR}
ENV OUT_DIR=${OUT_DIR}
ENV MPLCONFIGDIR=/tmp/

# set default shell
ENV SHELL=/bin/bash

CMD ["/phgasnets/src/RUNME.sh"]
