# open-x-database

*omxdb* is a key-value storage library based on Google LevelDB 
that provides an ordered mapping from string keys to string values.

## How to build

**Note** You need compiler with support c++20 standard. 
In this example we are going to use clang-13, but you can try your favourite compiler as well.

Clone LevelDB

```shell
git clone git@github.com:google/leveldb.git

cd leveldb

git checkout -b 1.23 1.23

git submodule update --init --recursive
```

Build and install *LevelDB*

```shell
mkdir build && cd build

cmake .. -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
  -DLEVELDB_BUILD_TESTS=OFF \
  -DLEVELDB_BUILD_BENCHMARKS=OFF \
  -DLEVELDB_INSTALL=ON \
  -DCMAKE_C_COMPILER=/usr/bin/clang-13 \
  -DCMAKE_CXX_COMPILER=/usr/bin/clang++-13 \
  -DCMAKE_INSTALL_PREFIX=/home/tikhon/libs/leveldb_v1_23
  
cmake --build . --config Release --target install -- -j 4
```

Clone *omxdb*

```shell
git clone git@github.com:tantskii/open-x-database.git

cd open-x-database
```

Build and install *omxdb*

```shell
mkdir build && cd build

cmake .. -DCMAKE_BUILD_TYPE=Release \
  -DLEVELDB_ROOT=/home/tikhon/libs/leveldb_v1_23 \
  -DCMAKE_C_COMPILER=/usr/bin/clang-13 \
  -DCMAKE_CXX_COMPILER=/usr/bin/clang++-13 \
  -DCMAKE_INSTALL_PREFIX=/home/tikhon/libs/omxdb_v1_0_0
  
cmake --build . --config Release --target install -- -j 4

```
