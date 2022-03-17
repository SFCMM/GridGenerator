# V1.1.0 (Q2/23)

## Planned:

- Add gui based on schema
- Add Paraview-Plugin for HDF5 format
- Read geometry in parallel
- Generate parallel geometry
- use CGAL algorithms

# V1.0.0 (Q1/23)

## Planned:

- Increase coverage to >90%
- Add schema check for the json file.
- Add LZMA2 compression options, https://github.com/google/brotli, https://github.com/schnaader/precomp-cpp
- Add messagePack, https://amzn.github.io/ion-docs/

# V0.2.0  (Q4/22)

## Planned:

- Save as unstructured VTK grid.
- Fix duplicate points in STLs.
- Add option to scale and translate STls.
- add more geometries star, donut, bipyramids
- add benchmark support for scalings
- move kdtree to common
- move boundingbox to common
- move io to common

# V0.1.0 (Q2/22)

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
- move more functionality into bounding box
- refactor kdtree
- refactor bounding boxes
- allow type changes in VTK output
- cleanup vtk io code
- more output values (properties)
- more complex filters for output (multiple filters, options)
- move all configuration function stuff to functions.h::config
- limit buffer size during IO writing to buffersize not cell count
- set log file name
- Bug: Some how just calling ofstream.write() yields binary differences???

# V0.0.4 (Q1/22)

- Save HDF5 grid format.
- write paraview plugin.

### Moved from V0.0.3:
- Save svg of the mesh. (4)
- add more geometries pyramid, cone, tube(4)
- add circular container based on vector... (3)

- Generate porous mesh provided with some porosity value.(2)

- check geometries are touching(2)

- add documentation for the configuration files (4)
- add tutorial (5)

- add test for 1D, 2D, 3D (2)
- add UnitTest and testcases for grid generation (2)

- Setup valgrind and sanitize tests (4)
- activate profile guided optimization (3)

- Run coverage within CI (4)
- Add doxygen generation to CI (4)
- add performance testing results to CI (5)
- Add error for missing documentation (6)
- Add python script to analyse performance. (5)

- fix all warnings (4)
- cleanup todo items (5)
- check for stuff that needs refactoring (6)
- remove config:: namespace since it is redundant

## Ongoing:

- Implement MPI communication.
- compare result of testcases

- allow subtracting geometry


## Done:

### Features
- allow to generate grids with nodes on the surface of the geometry (for boxes)

### Buildsystem

### Testing
- activate backtracing
- automatically run testcases

### Usability

### IO
- output data columns in binary as well for VTK binary files
- allow defining the output values that are written to the point files
- more output filters have been added (partitionLvl, lowestLvl, targetLvl)

### Performance

### Bugs
-fix some small typos in binary vtk code

### Documentation

### Refactoring

- introduce type for bounding boxes
- small cleanup in IO code
- refactor configuration file loading code

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