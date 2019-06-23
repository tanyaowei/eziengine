CUR_DIR=`pwd`
SCRIPT_PATH=`realpath $0`
BASE_PATH=`dirname $SCRIPT_PATH`
cd $BASE_PATH

. /var/emsdk/emsdk_env.sh
mkdir -p build
mkdir -p install
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_STANDARD=17 -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=$EMSCRIPTEN/cmake/Modules/Platform/Emscripten.cmake -DCMAKE_INSTALL_PREFIX=$BASE_PATH/install ..
make -j12
make install

cd $CUR_DIR
