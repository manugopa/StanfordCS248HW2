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
On `Simplification`, the smallest primitive was set to be when all remaining edges either have vertices that are degree 2 or less, or the same vertices. When `A` is singular, the midpoint of the lines is used. The America's Next Top 3D Model is included under the filename `$ROOT/model.dae`. 