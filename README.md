# Grid Generator

## Requirements

* GNU 10+ or CLang 10+
* OpenMPI v4+
* HDF5 v1.12+
* CMake 3.20+

## Compile

1) create build dir
   ```mkdir build```
2) execute cmake
   ```cmake ..```
3) compile the binary
   ```make -j 6```
4) run bench
   ```./gridgenerator --bench```