CUR_DIR=`pwd`
SCRIPT_PATH=`realpath $0`
BASE_PATH=`dirname $SCRIPT_PATH`
cd $BASE_PATH

source /var/common/emsdk/emsdk_env.sh
EMSCRIPTEN=`$EMSDK/upstream/emscripten`
mkdir -p build
mkdir -p install
cd build
time cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_STANDARD=17 -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=$EMSCRIPTEN/cmake/Modules/Platform/Emscripten.cmake -DCMAKE_INSTALL_PREFIX=$BASE_PATH/install ..
time make -j12
time make install

cd $CUR_DIR
