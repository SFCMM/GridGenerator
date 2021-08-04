# V1.0.0
- Increase coverage to >90%
- Add schema check for the json file.

# V0.2.0
- Save as unstructured VTK grid.
- Fix duplicate points in STLs.
- Add option to scale and translate STls.

# V0.1.0
## Planned
- Save HDF5 grid format.
- Patch refinement.
- Add python bindings.
- Add weighting methods.
- Add openmp SMD commands.
- Add more complex porosity options.
- Add CMD to load test configuration file.

# V0.0.3 (~22.8.21)
## Planned
- Generate porous mesh provided with some porosity value.
- Save svg of the mesh.
- Implement MPI communication.
- Open STL files.
- Add python script to analyse performance.
- Add error for documentation
- Add doxygen generation to CI
- (Optimize IO speed by manual buffering.)
- Split out common sim libs
- Setup valgrind and sanitize tests
- Run coverage within CI

# V0.0.2 (~8.8.21)
## Planned
- Boundary refinement.
- Write out VTK Point file (Binary/ASCII).
- Add OpenMP parallelization.
- Add bench cmd command.
- Add documentation in Readme.md of all configuration options.
- Build CI Pipeline from presets
- define geometries as inside or outside
- calculate bounding box
- additional output of geometry definition

## On-going

- Add CMake build presets for all build types.

## Done:

- Add Cmake preset
- Add option to generate initial doxygen documentation.
- Increase buffer size to 64kb.
- Run unit test from CI.
- Add analytical geometries cube, box and sphere.
- Add configuration options for geometries.

# V0.0.1

- Generate basic uniform grid
- Write out as ASCII Point file