# Mesh Repair Pipeline Coursework

My submission for COMP 5893M Modelling And Animation A1

## Architecture summary

* Each task is separated into its own executable, with no dependencies on each other or the provided renderer. This allows them to be used in any codebase, together or separately.
* The only universal required files are Cartesian3.h (based on the original Cartesian3.cpp/h in the provided renderer, adapted with additional functionality) and mesh_helpers.cpp/h (contains any functions used across multiple tasks)
* All files outputted by the executables are written to the current directory for convenience.
* The commands to build the executables are in compile.sh


## Compiling executables

* To compile the executables
```
chmod +x compile.sh
./compile.sh
```
* Outputs 5 executables: `face2faceindex`, `faceindex2directedEdge`, `manifold_test`, `mesh_repair` and `mesh_simplify`.


## Program summaries

### Face to face index

* Converts a .tri file to the .face format specified in the coursework brief
* To run:
```
./face2faceindex "filepath"
```

### Face index to directed edge

* Converts a .face file to the .diredge format specified in the coursework brief 
* To run:
```
./faceindex2directedEdge "filepath"
```

### Manifold test

* Iterates through each .tri file in a given directory specified via relative path, and returns:
    * Whether the mesh is manifold, and if not, the vertex or edge that caused the test to fail
    * The genus of each non-connected component in the mesh
* To run:
```
./manifold_test "relative_path_to_directory"
eg. ./manifold_test handout_models
```
* The results can be found in manifold_test_results.txt

### Mesh repair
* Repairs the mesh in hamish_40k_edit_2.tri by filling in the holes in the mesh to become manifold.
* Removes faces in the smallest component in the mesh to prevent component intersections

* To run:
```
./mesh_repair handout_models/hamish_40k_edit_2.tri
```
* Output is "hamish_40k_edit_2_repaired.tri"


### Mesh simplification
* Takes in a manifold mesh and performs greedy simplification based on gaussian curvature. Each iteration, the vertex with the lowest curvature that wouldn't violate the Eulerian condition for manifold surfaces is removed, and the affected faces and edges are rebuilt.
* Number of iterations is specified via a command line argument to provide maximum control over the output mesh to the artist.

* To run:
```
./mesh_simplify "filepath" "iterations"
```
* Output is "filename"_simplified.tri

## Author

Jacob Onion
sc23j2o@leeds.ac.uk