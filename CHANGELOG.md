# V1.1.0

## Planned:

- Add gui based on schema
- Add Paraview-Plugin for HDF5 format
- Read geometry in parallel
- Generate parallel geometry
- use CGAL algorithms

# V1.0.0

## Planned:

- Increase coverage to >90%
- Add schema check for the json file.
- Add LZMA2 compression options, https://github.com/google/brotli, https://github.com/schnaader/precomp-cpp
- Add messagePack, https://amzn.github.io/ion-docs/

# V0.2.0  (~Q4/21)

## Planned:

- Save as unstructured VTK grid.
- Fix duplicate points in STLs.
- Add option to scale and translate STls.
- add more geometries star, donut, bipyramids

# V0.1.0 (~Q3/21)

## Planned:

- Patch refinement.
- Add python bindings.
- Add weighting methods.
- Add openmp SMD commands.
- Add more complex porosity options.
- Add CMD to load test configuration file.
- add more geometries hat, prism, tetrahedron, Regular dodecahedron, Regular icosahedron

# V0.0.4 (~5.9.21)

- Save HDF5 grid format.

### Moved from V0.0.3:

- Implement MPI communication.
- Save svg of the mesh.
- Add python script to analyse performance.
- Add error for missing documentation
- add more geometries pyramid, cone, tube
- add tutorial
- add circular container based on vector...
- cleanup vtk io code
- activate profile guided optimization
- add performance testing results to CI
- introduce type for bounding boxes
- define more output filters (targetLvl etc.)
- activate backtrace
- Add doxygen generation to CI
- Run coverage within CI
- Generate porous mesh provided with some porosity value.
- (Optimize IO speed by manual buffering.)
- Setup valgrind and sanitize tests
- check geometries are touching
- allow defining the output values that are written to the point files
- BUG: donut_lowres.stl at level 6/4 produces some cells outside

# V0.0.3 (~31.8.21)

## Planned:
- save segment ids

### Moved from V0.0.2:

- Add OpenMP parallelization.
- Add documentation in Readme.md of all configuration options.
- Boundary refinement.
- Add bench cmd command.
- Write out VTK Point file (Binary).
- add UnitTest and testcases for grid generation


## Ongoing:

## Done:

### Features

- Define bodies of multiple geometries
- Open STL files.

### Buildsystem

### Testing

### Usability
- set filenames and paths

### IO

### Performance

### Bugs

- allow multiple geometries of same type

### Documentation

### Refactoring

- split out common functionality into common lib
- log.h
- timer.h
- merge cell properties

# V0.0.2 (8.8.21)

### Features

- Add analytical geometries cube, box and sphere.
- Define geometries as inside or outside

### Buildsystem

- Add Cmake presets

### Testing
- Run unit test from CI.
- Build CI Pipeline from presets


### Usability
- Add configuration options for geometries.
- Calculate bounding box
- Make output filter settable
- Additional output geometry


### IO
- Write out VTK Point file

### Performance
- Increase IO buffer size to 64kb.

### Bugs

### Documentation
- Add option to generate initial doxygen documentation.


# V0.0.1

- Generate basic uniform grid
- Write out as ASCII Point file