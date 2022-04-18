# open-x-database example

This example shows how to use omxdb.

## How to build

```shell

mkdir build && cd build

cmake .. -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_C_COMPILER=/usr/bin/clang-13 \
  -DCMAKE_CXX_COMPILER=/usr/bin/clang++-13 \
  -DOMXDB_ROOT=/home/tikhon/libs/omxdb
  
cmake --build . --config Release
  
./example_omxdb
```