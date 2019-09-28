CUR_DIR=`pwd`
SCRIPT_PATH=`realpath $0`
BASE_PATH=`dirname $SCRIPT_PATH`
cd $BASE_PATH

mkdir -p build
mkdir -p install
cd build
time cmake -DCMAKE_BUILD_TYPE=$1 -DCMAKE_CXX_STANDARD=17 -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX=$BASE_PATH/install ..
time make -j12
time make install

cd $CUR_DIR
