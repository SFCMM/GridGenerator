# V1.1.0

## Planned:

- Add gui based on schema
- Add Paraview-Plugin for HDF5 format
- Read geometry in parallel
- Generate parallel geometry

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
- Add doxygen generation to CI
- (Optimize IO speed by manual buffering.)
- Setup valgrind and sanitize tests
- Run coverage within CI
- introduce type for bounding boxes
- check geometries are touching
- add more geometries pyramid, cone, tube
- allow defining the output values
- define more output filters (targetLvl etc.)
- add performance testing results to CI
- set filenames and paths
- add tutorial
- activate profile guided optimization
- cleanup vtk io code
- activate backtrace
- add circular container based on vector...

### Moved from V0.0.2:

- Add OpenMP parallelization.
- Add documentation in Readme.md of all configuration options.
- Boundary refinement.
- Add bench cmd command.
- Write out VTK Point file (Binary).
- add UnitTest and testcases for grid generation

## Ongoing:

1) Open STL files.

## Done:

### Features

### Buildsystem

### Testing

### Usability

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

# V0.0.2 (~8.8.21)

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