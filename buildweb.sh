CUR_DIR=`pwd`
SCRIPT_PATH=`realpath $0`
BASE_PATH=`dirname $SCRIPT_PATH`
cd $BASE_PATH

cd /home/blazex47/Documents/projects/emsdk/
sh emsdk_env.sh
cd $BASE_PATH
mkdir -p build
mkdir -p install
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_STANDARD=17 -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=$EMSCRIPTEN/cmake/Modules/Platform/Emscripten.cmake -DCMAKE_INSTALL_PREFIX=$BASE_PATH/install ..
make -j8
make install

cd $CUR_DIR