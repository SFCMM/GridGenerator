# V1.1.0

## Planned:

- Add gui based on schema
- Add Paraview-Plugin for HDF5 format

# V1.0.0

## Planned:

- Increase coverage to >90%
- Add schema check for the json file.

# V0.2.0  (~Q4/21)

## Planned:

- Save as unstructured VTK grid.
- Fix duplicate points in STLs.
- Add option to scale and translate STls.
- add more geometries star, donut, bipyramids

# V0.1.0 (~Q3/21)

## Planned:

- Save HDF5 grid format.
- Patch refinement.
- Add python bindings.
- Add weighting methods.
- Add openmp SMD commands.
- Add more complex porosity options.
- Add CMD to load test configuration file.
- add more geometries hat, prism, tetrahedron, Regular dodecahedron, Regular icosahedron

### Moved from V0.0.3:

- Implement MPI communication.
- Save svg of the mesh.
- Add python script to analyse performance.
- Add error for documentation

# V0.0.3 (~22.8.21)

## Planned:

- Generate porous mesh provided with some porosity value.
- Open STL files.
- Add doxygen generation to CI
- (Optimize IO speed by manual buffering.)
- Split out common sim libs
- Setup valgrind and sanitize tests
- Run coverage within CI
- introduce type for bounding boxes
- check geometries are touching
- add more geometries pyramid, cone, tube
- allow defining the output values
- define more output filters (targetLvl etc.)

### Moved from V0.0.2:

- Add OpenMP parallelization.
- Add documentation in Readme.md of all configuration options.
- Boundary refinement.

# V0.0.2 (~8.8.21)

## Planned

- Write out VTK Point file (Binary/ASCII).
- Add bench cmd command.
- Build CI Pipeline from presets
- additional output of geometry definition
- add UnitTest and testcases for grid generation

## On-going:

- Add CMake build presets for all build types.

## Done:

- Add Cmake preset
- Add option to generate initial doxygen documentation.
- Increase buffer size to 64kb.
- Run unit test from CI.
- Add analytical geometries cube, box and sphere.
- Add configuration options for geometries.
- calculate bounding box
- define geometries as inside or outside
- make output filter settable

# V0.0.1

- Generate basic uniform grid
- Write out as ASCII Point file