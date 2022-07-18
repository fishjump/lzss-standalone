#!env bash

PROJ_DIR=$(dirname $(readlink -f $0))

cmake -B ${PROJ_DIR}/build ${PROJ_DIR}
make -C ${PROJ_DIR}/build