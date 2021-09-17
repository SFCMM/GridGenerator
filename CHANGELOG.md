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
- add benchmark support for scalings

# V0.1.0 (~Q3/21)

## Planned:

- Patch refinement.
- Add python bindings.
- Add weighting methods.
- Add openmp SMD commands.
- Add more complex porosity options.
- Add CMD to load test configuration file.
- add more geometries hat, prism, tetrahedron, Regular dodecahedron, Regular icosahedron
- improve OpenMP parallelisation
- cleanup VTK output functions
- bench command options like level etc.
- bench command validation of the grid
- generate hashkey for grids
- add better benchmark output
- allow to select boundary to be refined
- allow per boundary refinement settings
- boundary smoothing

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
- activate profile guided optimization
- add performance testing results to CI
- define more output filters (targetLvl etc.)
- activate backtrace
- Add doxygen generation to CI
- Run coverage within CI
- Generate porous mesh provided with some porosity value.
- (Optimize IO speed by manual buffering.)
- Setup valgrind and sanitize tests
- check geometries are touching
- add UnitTest and testcases for grid generation
- add documentation for the configuration files
- BUG: donut_lowres.stl at level 6/4 produces some cells outside

## Ongoing:

- allow defining the output values that are written to the point files
- introduce type for bounding boxes
- cleanup vtk io code

## Done:

### Features

### Buildsystem

### Testing

### Usability

### IO

### Performance

### Bugs

### Documentation

### Refactoring

# V0.0.3 (16.9.21)

### Features

- Define bodies of multiple geometries
- Open STL files.

### Usability

- set filenames and paths
- bench command
- simple boundary refinement

### IO

- Write out VTK Point file (Binary).

### Performance

- initial openMP parallelisation

### Bugs

- multiple geometries of same type not possible

### Documentation

- added details of the currently available commandline options

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