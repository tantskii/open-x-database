# open-x-database

*omxdb* is a key-value storage library based on Google LevelDB 
that provides an ordered mapping from string keys to string values.

## How to build

**Note** You need compiler with support c++20 standard. 
In this example we are going to use clang-13, but you can try your favourite compiler as well.

Clone *omxdb*

```shell
git clone git@github.com:tantskii/open-x-database.git

cd open-x-database

git submodule update --init --recursive
```

Build and install *omxdb*

```shell
mkdir build && cd build

cmake .. -DCMAKE_BUILD_TYPE=Release \
    -DLEVELDB_BUILD_TESTS=OFF  \
    -DLEVELDB_BUILD_BENCHMARKS=OFF \
    -DCMAKE_C_COMPILER=/usr/bin/clang-13 \
    -DCMAKE_CXX_COMPILER=/usr/bin/clang++-13 \
    -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
    -DCMAKE_INSTALL_PREFIX=/home/tikhon/omxdb
  
cmake --build . --config Release --target install -- -j 4

```
