#!/bin/bash

set -x

BUILD_TYPE=$1

cmake -S toucan/cmake/SuperBuild -B SuperBuild-$BUILD_TYPE -DCMAKE_INSTALL_PREFIX=$PWD/install -DCMAKE_PREFIX_PATH=$PWD/install -DCMAKE_BUILD_TYPE=$BUILD_TYPE
cmake --build SuperBuild-$BUILD_TYPE -j 4 --config $BUILD_TYPE

cmake -S toucan -B Build-$BUILD_TYPE -DCMAKE_INSTALL_PREFIX=$PWD/install -DCMAKE_PREFIX_PATH=$PWD/install -DCMAKE_BUILD_TYPE=$BUILD_TYPE
cmake --build Build-$BUILD_TYPE -j 4 --config $BUILD_TYPE
cmake --build Build-$BUILD_TYPE -j 4 --config $BUILD_TYPE --target test
