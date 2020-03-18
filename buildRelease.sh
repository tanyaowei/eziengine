#!/bin/bash

CUR_DIR=`pwd`
SCRIPT_PATH=`realpath $0`
BASE_PATH=`dirname $SCRIPT_PATH`
cd $BASE_PATH

mkdir -p build
mkdir -p install
conan install . -pr ../conan-utils/profiles/nodejs --install-folder=build --build=missing
conan build . --build-folder=build
#cmake -DCMAKE_CXX_COMPILER=/usr/bin/clang++ -DCMAKE_CC_COMPILER=/usr/bin/clang -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_STANDARD=17 -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX=$BASE_PATH/install ..
#make -j12
#make install

cd $CUR_DIR