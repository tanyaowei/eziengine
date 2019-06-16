CUR_DIR=`pwd`
SCRIPT_PATH=`realpath $0`
BASE_PATH=`dirname $SCRIPT_PATH`
cd $BASE_PATH

mkdir -p build
mkdir -p install
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_STANDARD=17 -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX=$BASE_PATH/install ..
make -j8
make install

cd $CUR_DIR