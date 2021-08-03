# Grid Generator

## Features
- Generate Cartesian mesh


## License
- Code is BSD v3 licensed
- Data and Documentation is CC-BY-NC ![license](https://creativecommons.org/licenses/by-nc/4.0/ "CC-BY-NC)")

## Requirements

* GNU 10+ or CLang 10+
* OpenMPI v4+
* HDF5 v1.12+

## Compile

1) create build dir
   ```mkdir build```
2) execute cmake
   ```cmake ..```
3) compile the binary
   ```make -j 6```
4) run bench
   ```./gridgenerator --bench```
