# This file is a shortcut for building and executing the sense project.

set -e 

mkdir -p build 
cd build
cmake ..
make -j5


test/logTest
