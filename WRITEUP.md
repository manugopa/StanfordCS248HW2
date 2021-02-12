# MeshEdit: Interactive Computer Graphics (CS 248) Assignment 2

Edwin Pan (edwinpan@stanford.edu)
Manu Gopakumar (manugopa@stanford.edu)

## Required Tasks Completed
### Local
- [x] FaceBevel
- [x] EdgeCollapse
- [x] EdgeFlip
- [x] EdgeSplit

### Global
- [x] Triangulation
- [x] LinearSubdivision
- [x] CatmullClarkSubdivision
- [x] Simplification


## Extra Tasks Completed
### Local
- [ ] EraseVertex
- [ ] EraseEdge
- [ ] VertexBevel
- [ ] EdgeBevel
- [ ] FaceCollapse

### Global
- [ ] LoopSubdivision - depends on EdgeSplit and EdgeFlip
- [ ] IsotropicRemeshing - depends on EdgeSplit, EdgeFlip, and EdgeCollapse


## Comments/Considerations
Simplification collapses the shape until any additional edge collapses would result in edges from different faces being collapsed onto each other or there are only 2 remaining faces. When 'A' is singular for simplification, the edge is collapsed to the best point from the midpoint and the two vertices.

 The America's Next Top 3D Model is included under the filename `$ROOT/model.dae`. 