
#include <queue>
#include <set>
#include <unordered_map>
#include <iostream>

#include "../geometry/halfedge.h"
#include "debug.h"

/* Note on local operation return types:

    The local operations all return a std::optional<T> type. This is used so that your
    implementation can signify that it does not want to perform the operation for
    whatever reason (e.g. you don't want to allow the user to erase the last vertex).

    An optional can have two values: std::nullopt, or a value of the type it is
    parameterized on. In this way, it's similar to a pointer, but has two advantages:
    the value it holds need not be allocated elsewhere, and it provides an API that
    forces the user to check if it is null before using the value.

    In your implementaiton, if you have successfully performed the operation, you can
    simply return the required reference:

            ... collapse the edge ...
            return collapsed_vertex_ref;

    And if you wish to deny the operation, you can return the null optional:

            return std::nullopt;

    Note that the stubs below all reject their duties by returning the null optional.
*/

/*
    This method should replace the given vertex and all its neighboring
    edges and faces with a single face, returning the new face.
 */
std::optional<Halfedge_Mesh::FaceRef> Halfedge_Mesh::erase_vertex(Halfedge_Mesh::VertexRef v) {

    (void)v;
    return std::nullopt;
}

/*
    This method should erase the given edge and return an iterator to the
    merged face.
 */
std::optional<Halfedge_Mesh::FaceRef> Halfedge_Mesh::erase_edge(Halfedge_Mesh::EdgeRef e) {

    (void)e;
    return std::nullopt;
}

/*
    This method should collapse the given edge and return an iterator to
    the new vertex created by the collapse.
*/
std::optional<Halfedge_Mesh::VertexRef> Halfedge_Mesh::collapse_edge(Halfedge_Mesh::EdgeRef e) {
    HalfedgeRef h0 = e->halfedge();
    VertexRef v0 = h0->vertex();
    FaceRef f0 = h0->face();

    HalfedgeRef h1 = h0->twin();
    VertexRef v1 = h1->vertex();
    FaceRef f1 = h1->face();

    if ((v1->degree() <= 2) || (v0->degree() <= 2) || (v0 == v1)){
        return std::nullopt;
    }

    // Collect edges referencing v1
    std::vector<HalfedgeRef> v1_halfedges;
    HalfedgeRef hIterator = h1;
    while (hIterator->twin()->next() != h1) {
        hIterator = hIterator->twin()->next();
        v1_halfedges.push_back(hIterator);
    }
    v1_halfedges.push_back(hIterator);


    // Delete adjacent faces and one edge if the faces are triangles; reassign halfedges of face otherwise
    bool f0_triangle = (f0->degree() == 3);
    bool f1_triangle = (f1->degree() == 3);
    HalfedgeRef h2;
    HalfedgeRef h2_twin;
    EdgeRef e2;
    FaceRef f2;
    if (f0_triangle) {
        // Store for deletion
        h2 = h0->next();
        e2 = h2->edge();
        h2_twin = h2->twin();
        f2 = h2_twin->face();

        // Update remaining face and vertex
        HalfedgeRef h4 = h2->next();
        f2->halfedge() = h4;
        VertexRef v2 = h4->vertex();
        v2->halfedge() = h4;

        // Update incoming edge in combined face for collapsed edge
        h4->next() = h2_twin->next();
        h4->face() = f2;
        h4->twin() = h4->twin();
        h4->vertex() = v2;
        h4->edge() = h4->edge();

        // Update outgoing edge in combined face for collapsed edge
        HalfedgeRef h6 = h2_twin;
        while (h6->next() != h2_twin) {
            h6 = h6->next();
        }
        h6->next() = h4;
        h6->twin() = h6->twin();
        h6->face() = f2;
        h6->vertex() = h6->vertex();
        h6->edge() = h6->edge();
    } else {
        f0->halfedge() = h0->next();
        HalfedgeRef h2 = h0->next();
        HalfedgeRef h4 = h0->next();
        while (h4->next() != h0) {
            h4 = h4->next();
        }
        h4->next() = h2;
    }
    HalfedgeRef h3;
    HalfedgeRef h3_twin;
    EdgeRef e3;
    FaceRef f3;
    if (f1_triangle) {
        // Store for deletion
        h3 = h1->next();
        e3 = h3->edge();
        h3_twin = h3->twin();
        f3 = h3_twin->face();

        // Update remaining face and vertex
        HalfedgeRef h5 = h3->next();
        f3->halfedge() = h5;
        VertexRef v3 = h5->vertex();
        v3->halfedge() = h5;

        // Update incoming edge in combined face for collapsed edge
        h5->next() = h3_twin->next();
        h5->face() = f3;
        h5->twin() = h5->twin();
        h5->vertex() = v3;
        h5->edge() = h5->edge();

        // Update outgoing edge in combined face for collapsed edge
        HalfedgeRef h7 = h3_twin;
        while (h7->next() != h3_twin) {
            h7 = h7->next();
        }
        h7->next() = h5;
        h7->twin() = h7->twin();
        h7->face() = f3;
        h7->vertex() = h7->vertex();
        h7->edge() = h7->edge();

        // Update v0 halfedge
        v0->halfedge() = h5->twin();
    } else {
        f1->halfedge() = h1->next();
        HalfedgeRef h3 = h1->next();
        HalfedgeRef h5 = h1->next();
        while (h5->next() != h1) {
            h5 = h5->next();
        }
        h5->next() = h3;
    }

    while (!v1_halfedges.empty()) {
        hIterator = v1_halfedges.back();
        hIterator->vertex() = v0;
        v1_halfedges.pop_back();
    }
    v0->pos = center_of(e);

    if (f0_triangle) {
        erase(h2_twin);
        erase(h2);
        erase(f0);
        erase(e2);
    }
    if (f1_triangle) {
        erase(h3_twin);
        erase(h3_twin);
        erase(h3);
        erase(f1);
        erase(e3);
    }
    erase(h0);
    erase(h1);
    erase(e);
    erase(v1);
    return v0;

}

/*
    This method should collapse the given face and return an iterator to
    the new vertex created by the collapse.
*/
std::optional<Halfedge_Mesh::VertexRef> Halfedge_Mesh::collapse_face(Halfedge_Mesh::FaceRef f) {

    (void)f;
    return std::nullopt;
}

/*
    This method should flip the given edge and return an iterator to the
    flipped edge.
*/
std::optional<Halfedge_Mesh::EdgeRef> Halfedge_Mesh::flip_edge(Halfedge_Mesh::EdgeRef e) {

    //Handle Edge Case: e is an outer edge
    if (e->on_boundary()) {
        return std::nullopt;
    }
    //// Load the relevant data structures
    // HALFEDGES
    HalfedgeRef h0 = e->halfedge();
    HalfedgeRef h1 = h0->next();
    HalfedgeRef h1n = h1->next();
    HalfedgeRef h2 = h1->next();
    while (h2->next() != h0) {
        h2 = h2->next();
    }
    HalfedgeRef h3 = h0->twin();
    HalfedgeRef h4 = h3->next();
    HalfedgeRef h4n = h4->next();
    HalfedgeRef h5 = h4->next();
    while (h5->next() != h3) {
        h5 = h5->next();
    }
    HalfedgeRef h6 = h1->twin();
    HalfedgeRef h7 = h2->twin();
    HalfedgeRef h8 = h4->twin();
    HalfedgeRef h9 = h5->twin();

    // VERTICES
    VertexRef v0 = h0->vertex();
    VertexRef v1 = h3->vertex();
    VertexRef v2 = h8->vertex();
    VertexRef v3 = h6->vertex();

    // EDGES
    EdgeRef e1 = h5->edge();
    EdgeRef e2 = h4->edge();
    EdgeRef e3 = h2->edge();
    EdgeRef e4 = h1->edge();

    // FACES
    FaceRef f0 = h0->face();
    FaceRef f1 = h3->face();

    //// Update data relationships
    // HALFEDGES
    h0->next() = h1n;
    h0->twin() = h3;
    h0->vertex() = v2;
    h0->edge() = e;
    h0->face() = f0;

    h1->next() = h3;
    h1->twin() = h6;
    h1->vertex() = v1;
    h1->edge() = e4;
    h1->face() = f1;

    h2->next() = h4;
    h2->twin() = h7;
    h2->vertex() = h2->vertex();
    h2->edge() = e3;
    h2->face() = f0;

    h3->next() = h4n;
    h3->twin() = h0;
    h3->vertex() = v3;
    h3->edge() = e;
    h3->face() = f1;

    h4->next() = h0;
    h4->twin() = h8;
    h4->vertex() = v0;
    h4->edge() = e2;
    h4->face() = f0;

    h5->next() = h1;
    h5->twin() = h9;
    h5->vertex() = h5->vertex();
    h5->edge() = e1;
    h5->face() = f1;

    h6->next() = h6->next();
    h6->twin() = h1;
    h6->vertex() = v3;
    h6->edge() = e4;
    h6->face() = h6->face();

    h7->next() = h7->next();
    h7->twin() = h2;
    h7->vertex() = v0;
    h7->edge() = e3;
    h7->face() = h7->face();

    h8->next() = h8->next();
    h8->twin() = h4;
    h8->vertex() = v2;
    h8->edge() = e2;
    h8->face() = h8->face();

    h9->next() = h9->next(); // didn't change, but set it anyway!
    h9->twin() = h5;
    h9->vertex() = v1;
    h9->edge() = e1;
    h9->face() = h9->face(); // didn't change, but set it anyway!

    // VERTICES: assign with an outgoing halfedge
    v0->halfedge() = h4;
    v1->halfedge() = h1;
    v2->halfedge() = h8;
    v3->halfedge() = h6;

    // EDGES: assign with an adjacent halfedge
    e->halfedge() = h0;
    e1->halfedge() = h9;
    e2->halfedge() = h4;
    e3->halfedge() = h7;

    // FACES: assign with an interior halfedge
    f0->halfedge() = h0;
    f1->halfedge() = h3;

    return e;
}

/*
    This method should split the given edge and return an iterator to the
    newly inserted vertex. The halfedge of this vertex should point along
    the edge that was split, rather than the new edges.
*/
std::optional<Halfedge_Mesh::VertexRef> Halfedge_Mesh::split_edge(Halfedge_Mesh::EdgeRef e) {
    //Handle Edge Case: e is an outer edge
    HalfedgeRef h0;
    if (e->on_boundary()) {
        if (e->halfedge()->is_boundary()) {
            h0 = e->halfedge()->twin();
        } else {
            h0 = e->halfedge();
        }
    } else {
        h0 = e->halfedge();
    }
    //// Load the relevant data structures
    // HALFEDGES
    HalfedgeRef h1 = h0->next();
    HalfedgeRef h1n = h1->next();
    HalfedgeRef h2 = h1->next();
    while (h2->next() != h0) {
        h2 = h2->next();
    }
    if (h1n != h2) { // Handle non triangular adjacent face
        return std::nullopt;
    }
    HalfedgeRef h3 = h0->twin();
    HalfedgeRef h6 = h1->twin();
    HalfedgeRef h7 = h2->twin();
    HalfedgeRef h4;
    HalfedgeRef h4n;
    HalfedgeRef h5;
    HalfedgeRef h8;
    HalfedgeRef h9;
    if (!(e->on_boundary())) {
        h4 = h3->next();
        h4n = h4->next();
        h5 = h4->next();
        while (h5->next() != h3) {
            h5 = h5->next();
        }
        if (h4n != h5) { // Handle non triangular adjacent face
            return std::nullopt;
        }
        h8 = h4->twin();
        h9 = h5->twin();
    }

    // VERTICES
    VertexRef v0 = h0->vertex();
    VertexRef v1 = h3->vertex();
    VertexRef v3 = h6->vertex();
    VertexRef v2;
    if (!(e->on_boundary())) {
        v2 = h8->vertex();
    }

    // EDGES
    EdgeRef e1;
    EdgeRef e2;
    if (!(e->on_boundary())) {
        e1 = h5->edge();
        e2 = h4->edge();
    }
    EdgeRef e3 = h2->edge();
    EdgeRef e4 = h1->edge();

    // FACES
    FaceRef f0 = h0->face();
    FaceRef f1;
    if (!(e->on_boundary())) {
        f1 = h3->face();
    }

    //// Add new elements
    // HALFEDGES
    HalfedgeRef h10 = new_halfedge();
    HalfedgeRef h11 = new_halfedge();
    HalfedgeRef h12 = new_halfedge();
    HalfedgeRef h13 = new_halfedge();
    HalfedgeRef h14;
    HalfedgeRef h15;
    if (!(e->on_boundary())) {
        h14 = new_halfedge();
        h15 = new_halfedge();
    }

    // VERTICES
    VertexRef v4 = new_vertex();

    // EDGES
    EdgeRef e5 = new_edge();
    EdgeRef e6 = new_edge();
    EdgeRef e7;
    if (!(e->on_boundary())) {
        e7 = new_edge();
    }

    // FACES
    FaceRef f2 = new_face();
    FaceRef f3;
    if (!(e->on_boundary())) {
        f3 = new_face();
    }

    //// Update data relationships
    // HALFEDGES
    h0->next() = h1;
    h0->twin() = h3;
    h0->vertex() = v4;
    h0->edge() = e;
    h0->face() = f0;

    h1->next() = h10;
    h1->twin() = h6;
    h1->vertex() = v1;
    h1->edge() = e4;
    h1->face() = f0;

    h2->next() = h12;
    h2->twin() = h7;
    h2->vertex() = h2->vertex();
    h2->edge() = e3;
    h2->face() = f2;

    if (e->on_boundary()) {
        h13->next() = h3->next();
    } else {
        h13->next() = h4;
    }
    h13->twin() = h12;
    h13->vertex() = v4;
    h13->edge() = e6;
    if (e->on_boundary()) {
        h13->face() = h3->face();  // boundary face
    } else {
        h13->face() = f3;
    }

    if (e->on_boundary()) {
        h3->next() = h13;
    } else {
        h3->next() = h15;
    }
    h3->twin() = h0;
    h3->vertex() = v1;
    h3->edge() = e;
    if (e->on_boundary()) {
        h3->face() = h3->face();  // boundary face
    } else {
        h3->face() = f1;
    }

    if (!(e->on_boundary())) {
        h4->next() = h14;
        h4->twin() = h8;
        h4->vertex() = v0;
        h4->edge() = e2;
        h4->face() = f3;

        h5->next() = h3;
        h5->twin() = h9;
        h5->vertex() = h5->vertex();
        h5->edge() = e1;
        h5->face() = f1;
    }

    h6->next() = h6->next();
    h6->twin() = h1;
    h6->vertex() = v3;
    h6->edge() = e4;
    h6->face() = h6->face();

    h7->next() = h7->next();
    h7->twin() = h2;
    h7->vertex() = v0;
    h7->edge() = e3;
    h7->face() = h7->face();

    if (!(e->on_boundary())) {
        h8->next() = h8->next();
        h8->twin() = h4;
        h8->vertex() = v2;
        h8->edge() = e2;
        h8->face() = h8->face();

        h9->next() = h9->next(); // didn't change, but set it anyway!
        h9->twin() = h5;
        h9->vertex() = v1;
        h9->edge() = e1;
        h9->face() = h9->face(); // didn't change, but set it anyway!
    }

    h10->next() = h0;
    h10->twin() = h11;
    h10->vertex() = v3;
    h10->edge() = e5;
    h10->face() = f0;

    h11->next() = h1n;
    h11->twin() = h10;
    h11->vertex() = v4;
    h11->edge() = e5;
    h11->face() = f2;

    h12->next() = h11;
    h12->twin() = h13;
    h12->vertex() = v0;
    h12->edge() = e6;
    h12->face() = f2;

    if (!(e->on_boundary())) {
        h14->next() = h13;
        h14->twin() = h15;
        h14->vertex() = v2;
        h14->edge() = e7;
        h14->face() = f3;

        h15->next() = h4n;
        h15->twin() = h14;
        h15->vertex() = v4;
        h15->edge() = e7;
        h15->face() = f1;
    }

    // VERTICES: assign with an outgoing halfedge
    v0->halfedge() = h7;
    v1->halfedge() = h1;
    v3->halfedge() = h10;
    v4->halfedge() = h0;
    v4->pos = (v1->pos)/2 + (v0->pos)/2;
    if (!(e->on_boundary())) {
        v2->halfedge() = h14;
    }

    // EDGES: assign with an adjacent halfedge
    e->halfedge() = h0;
    e3->halfedge() = h2;
    e4->halfedge() = h1;
    e5->halfedge() = h10;
    e6->halfedge() = h12;
    if (!(e->on_boundary())) {
        e1->halfedge() = h5;
        e2->halfedge() = h4;
        e7->halfedge() = h14;
    }

    // FACES: assign with an interior halfedge
    f0->halfedge() = h0;
    f2->halfedge() = h11;
    if (!(e->on_boundary())) {
        f1->halfedge() = h5;
        f3->halfedge() = h4;
    }

    return v4;
}

/* Note on the beveling process:

    Each of the bevel_vertex, bevel_edge, and bevel_face functions do not represent
    a full bevel operation. Instead, they should update the _connectivity_ of
    the mesh, _not_ the positions of newly created vertices. In fact, you should set
    the positions of new vertices to be exactly the same as wherever they "started from."

    When you click on a mesh element while in bevel mode, one of those three functions
    is called. But, because you may then adjust the distance/offset of the newly
    beveled face, we need another method of updating the positions of the new vertices.

    This is where bevel_vertex_positions, bevel_edge_positions, and
    bevel_face_positions come in: these functions are called repeatedly as you
    move your mouse, the position of which determins the normal and tangent offset
    parameters. These functions are also passed an array of the original vertex
    positions: for  bevel_vertex, it has one element, the original vertex position,
    for bevel_edge,  two for the two vertices, and for bevel_face, it has the original
    position of each vertex in halfedge order. You should use these positions, as well
    as the normal and tangent offset fields to assign positions to the new vertices.

    Finally, note that the normal and tangent offsets are not relative values - you
    should compute a particular new position from them, not a delta to apply.
*/

/*
    This method should replace the vertex v with a face, corresponding to
    a bevel operation. It should return the new face.  NOTE: This method is
    responsible for updating the *connectivity* of the mesh only---it does not
    need to update the vertex positions.  These positions will be updated in
    Halfedge_Mesh::bevel_vertex_positions (which you also have to
    implement!)
*/
std::optional<Halfedge_Mesh::FaceRef> Halfedge_Mesh::bevel_vertex(Halfedge_Mesh::VertexRef v) {

    // Reminder: You should set the positions of new vertices (v->pos) to be exactly
    // the same as wherever they "started from."

    (void)v;
    return std::nullopt;
}

/*
    This method should replace the edge e with a face, corresponding to a
    bevel operation. It should return the new face. NOTE: This method is
    responsible for updating the *connectivity* of the mesh only---it does not
    need to update the vertex positions.  These positions will be updated in
    Halfedge_Mesh::bevel_edge_positions (which you also have to
    implement!)
*/
std::optional<Halfedge_Mesh::FaceRef> Halfedge_Mesh::bevel_edge(Halfedge_Mesh::EdgeRef e) {

    // Reminder: You should set the positions of new vertices (v->pos) to be exactly
    // the same as wherever they "started from."

    (void)e;
    return std::nullopt;
}

/*
    This method should replace the face f with an additional, inset face
    (and ring of faces around it), corresponding to a bevel operation. It
    should return the new face.  NOTE: This method is responsible for updating
    the *connectivity* of the mesh only---it does not need to update the vertex
    positions. These positions will be updated in
    Halfedge_Mesh::bevel_face_positions (which you also have to
    implement!)
*/
std::optional<Halfedge_Mesh::FaceRef> Halfedge_Mesh::bevel_face(Halfedge_Mesh::FaceRef f) {

    // Reminder: You should set the positions of new vertices (v->pos) to be exactly
    // the same as wherever they "started from."

    // (void)f;

    // -------------------------------------------------------------------------------------------
    // --- See diagram in {DIAGRAM_FNAME}.md for visual mapping between mesh objects & indices ---
    // -------------------------------------------------------------------------------------------
    std::cout<<"Yay we're beveling! " << std::endl;

    // Don't bevel a boundary...
    if(f->is_boundary()){return f;}

    // Bank of edges, halfedges, faces, vertices, and edges that must be connected into the new mesh
    HalfedgeRef halfedge_iter_ref = f->halfedge();
    std::vector<HalfedgeRef> original_halfedges; // Keep track of face-poly half_edges 
    std::vector<FaceRef> new_faces; // Should be one new face for each face-poly degree
    std::vector<HalfedgeRef> new_halfedges; // Will need to fill this with 4 HE per face-poly degree
    std::vector<VertexRef> new_vertices; // Should be one new vertex for each face-poly degree
    std::vector<EdgeRef> new_edges; // Should be two edges per new face

    // Utilities -- should be constant
    int NUM_EDGES_PER_BFACE = 2; // Number of new edges added per bevel-face
    int NUM_HALFEDGES_PER_BFACE = 4; // Number of half-edges required to loop around one bevel-face

    // Loop around the given face f, after performing one step first
    // Locates all half edges that need new faces
    do
    {
        // looping utils & face-poly halfedge tracking
        original_halfedges.push_back(halfedge_iter_ref);
        halfedge_iter_ref = halfedge_iter_ref->next();

        // create all the faces, edges, vertices, and half-edge objects needed
        new_faces.push_back(new_face());
        new_vertices.push_back(new_vertex());
        for(int i=0; i<NUM_EDGES_PER_BFACE;i++)
            new_edges.push_back(new_edge()); // [horizontal_edge, vertical_edge]
        for(int i=0; i<NUM_HALFEDGES_PER_BFACE; i++)
            new_halfedges.push_back(new_halfedge()); // [right, top, left, up]x4
    }while(halfedge_iter_ref != f->halfedge());

    int N = original_halfedges.size(); // Note: this should always be the same as f->degree()
    for(size_t i=0; i < f->degree(); i++)
    {
        int halfedge_idx = i*NUM_HALFEDGES_PER_BFACE; // Since there are NUM_HALFEDGES_PER_BFACE number of halfedges per iteration unit (bevel faces)
        int edge_idx = i*NUM_EDGES_PER_BFACE; // Since there are NUM_EDGES_PER_BFACE number of edges added per iteration unit (bevel faces)

        // Upkeep: set new vertices to their respective "starting positions" & one possible associated half-edge
        new_vertices[i]->pos = original_halfedges[i]->vertex()->pos;
        new_vertices[i]->halfedge() = new_halfedges[halfedge_idx+3]; // Chose to set it to the halfedges within f

        // Upkeep: set new edges to one possible associated half-edge
        new_edges[edge_idx]->halfedge() = new_halfedges[halfedge_idx+3]; // Chose to set it to the halfedges within f
        new_edges[edge_idx+1]->halfedge() = new_halfedges[halfedge_idx];

        // Upkeep: set new faces to one of possible associated half-edges
        new_faces[i]->halfedge() = new_halfedges[halfedge_idx];

        // bottom (defined as the original halfedge)
        original_halfedges[i]->face() = new_faces[i]; // Assign new face
        original_halfedges[i]->next() = new_halfedges[halfedge_idx]; // Begin new loop

        // right (index%NUM_HALFEDGES_PER_BFACE=0)
        new_halfedges[halfedge_idx]->twin() = new_halfedges[((i+1)*NUM_HALFEDGES_PER_BFACE+2)%(N*NUM_HALFEDGES_PER_BFACE)];
        new_halfedges[halfedge_idx]->next() = new_halfedges[halfedge_idx+1];
        new_halfedges[halfedge_idx]->vertex() = original_halfedges[(i+1)%N]->vertex();
        new_halfedges[halfedge_idx]->edge() = new_edges[edge_idx+1]; // vertical
        new_halfedges[halfedge_idx]->face() = new_faces[i];

        // top (index%NUM_HALFEDGES_PER_BFACE=1)
        new_halfedges[halfedge_idx+1]->twin() = new_halfedges[halfedge_idx+3]; // Shared within iteration unit (up)
        new_halfedges[halfedge_idx+1]->next() = new_halfedges[halfedge_idx+2];
        new_halfedges[halfedge_idx+1]->vertex() = new_vertices[(i+1)%N];
        new_halfedges[halfedge_idx+1]->edge() = new_edges[edge_idx]; // horizontal
        new_halfedges[halfedge_idx+1]->face() = new_faces[i];

        // left (index%NUM_HALFEDGES_PER_BFACE=2)
        i == 0 ? new_halfedges[halfedge_idx+2]->twin() = new_halfedges[((N-1)*NUM_HALFEDGES_PER_BFACE)] : new_halfedges[halfedge_idx+2]->twin() = new_halfedges[(i-1)*NUM_HALFEDGES_PER_BFACE]; // First halfedge of last iteration unit if at first iteration unit, otherwise, prev iteration unit's first halfedge
        new_halfedges[halfedge_idx+2]->next() = original_halfedges[i];
        new_halfedges[halfedge_idx+2]->vertex() = new_vertices[i];
        i == 0 ? new_halfedges[halfedge_idx+2]->edge() = new_edges[N*NUM_EDGES_PER_BFACE-1] : new_halfedges[halfedge_idx+2]->edge() = new_edges[edge_idx-1]; // wrap to last-vertical-edge if at first face, otherwise vertical
        new_halfedges[halfedge_idx+2]->face() = new_faces[i];

        // up (index%NUM_HALFEDGES_PER_BFACE=3)
        new_halfedges[halfedge_idx+3]->twin() = new_halfedges[halfedge_idx+1]; // Shared within iteration unit (top)
        new_halfedges[halfedge_idx+3]->next() = new_halfedges[(((i+1)*NUM_HALFEDGES_PER_BFACE)%(N*NUM_HALFEDGES_PER_BFACE))+3];
        new_halfedges[halfedge_idx+3]->vertex() = new_vertices[i];
        new_halfedges[halfedge_idx+3]->edge() = new_edges[edge_idx]; // horizontal
        new_halfedges[halfedge_idx+3]->face() = f;
    }

    // // DEBUG -- Twin twin is not self!
    // std::cout << "CHECKING TWINS" << std::endl;
    // int check_idx = 1;
    // std::cout << "HE#" << check_idx << ": " << new_halfedges[check_idx]->id() << std::endl;

    // for(int i=0; i < 4*N; i++)
    // {
    //     std::cout << "AT HALF-EDGE: (" << i << "," << new_halfedges[i]->id() << ") \t twin=(" << new_halfedges[i]->twin()->id() << ") \t" << "twin^2=(" << new_halfedges[i]->twin()->twin()->id() << ")" << std::endl;
    // }

    // // DEBUG -- A half-edge is the next of nothing!
    // std::cout << "--- edge#-to-ID# Pairing ---" << std::endl;
    // for(int i=0; i < 4*N; i++)
    // {
    //     std::cout << "HE#" << i << ": " << new_halfedges[i]->id() << std::endl;
    // }
    // std::cout << "--- -------------------- ---" << std::endl;
    // for(int i=0; i < 4; i++)
    // {
    //     std::cout << "OG_HE#" << i << ": " << original_halfedges[i]->id() << std::endl;
    // }
    // std::cout << "--- -------------------- ---" << std::endl;

    // for(int i=0; i < 4*N; i++)
    // {
    //     std::cout << "HE#" << i << "->next()= " << new_halfedges[i]->next()->id() << std::endl;
    // }
    // std::cout << "--- -------------------- ---" << std::endl;
    // for(int i=0; i < 4; i++)
    // {
    //     std::cout << "OG_HE#" << i << "->next()= " << original_halfedges[i]->next()->id() << std::endl;
    // }

    // // DEBUG -- Unexplained segfault :(
    // std::cout << "--- Vertex#-to-ID# Pairing ---" << std::endl;
    // for(int i=0; i < 4; i++)
    // {
    //     std::cout << "V#" << i << ": " << new_vertices[i]->id() << std::endl;
    // }
    // for(int i=0; i < 4; i++)
    // {
    //     std::cout << "OG_V#" << i << ": " << original_halfedges[i]->vertex()->id() << std::endl;
    // }
    // std::cout << "--- -------------------- ---" << std::endl;

    // std::cout << "--- Check all Vertices ---" << std::endl;
    // for(int i=0; i < 4*N; i++)
    // {
    //     std::cout << "HE#" << i << ": " << new_halfedges[i]->vertex()->id() << std::endl;
    // }
    // std::cout << "--- -------------------- ---" << std::endl;



    // Give f it's new starting half-edge
    f->halfedge() = new_halfedges[3];

    // HalfedgeRef test_h = f->halfedge();
    // do {
    //     test_h = test_h->next();
    //     std::cout << "Tracing: " << test_h->id() << std::endl;
    // } while(test_h != f->halfedge());

    // std::cout << "Yay! Done beveling: " << f->id() << std::endl;
    // std::cout << "Yay! Done beveling: " << f->halfedge()->id() << std::endl;

    return f;
}

/*
    Compute new vertex positions for the vertices of the beveled vertex.

    These vertices can be accessed via new_halfedges[i]->vertex()->pos for
    i = 1, ..., new_halfedges.size()-1.

    The basic strategy here is to loop over the list of outgoing halfedges,
    and use the original vertex position and its associated outgoing edge
    to compute a new vertex position along the outgoing edge.
*/
void Halfedge_Mesh::bevel_vertex_positions(const std::vector<Vec3>& start_positions,
                                           Halfedge_Mesh::FaceRef face, float tangent_offset) {

    std::vector<HalfedgeRef> new_halfedges;
    auto h = face->halfedge();
    do {
        new_halfedges.push_back(h);
        h = h->next();
    } while(h != face->halfedge());

    (void)new_halfedges;
    (void)start_positions;
    (void)face;
    (void)tangent_offset;
}

/*
    Compute new vertex positions for the vertices of the beveled edge.

    These vertices can be accessed via new_halfedges[i]->vertex()->pos for
    i = 1, ..., new_halfedges.size()-1.

    The basic strategy here is to loop over the list of outgoing halfedges,
    and use the preceding and next vertex position from the original mesh
    (in the orig array) to compute an offset vertex position.

    Note that there is a 1-to-1 correspondence between halfedges in
    newHalfedges and vertex positions
    in orig.  So, you can write loops of the form

    for(size_t i = 0; i < new_halfedges.size(); i++)
    {
            Vector3D pi = start_positions[i]; // get the original vertex
            position corresponding to vertex i
    }
*/
void Halfedge_Mesh::bevel_edge_positions(const std::vector<Vec3>& start_positions,
                                         Halfedge_Mesh::FaceRef face, float tangent_offset) {

    std::vector<HalfedgeRef> new_halfedges;
    auto h = face->halfedge();
    do {
        new_halfedges.push_back(h);
        h = h->next();
    } while(h != face->halfedge());

    (void)new_halfedges;
    (void)start_positions;
    (void)face;
    (void)tangent_offset;
}

/*
    Compute new vertex positions for the vertices of the beveled face.

    These vertices can be accessed via new_halfedges[i]->vertex()->pos for
    i = 1, ..., new_halfedges.size()-1.

    The basic strategy here is to loop over the list of outgoing halfedges,
    and use the preceding and next vertex position from the original mesh
    (in the start_positions array) to compute an offset vertex
    position.

    Note that there is a 1-to-1 correspondence between halfedges in
    new_halfedges and vertex positions
    in orig. So, you can write loops of the form

    for(size_t i = 0; i < new_halfedges.size(); i++)
    {
            Vec3 pi = start_positions[i]; // get the original vertex
            position corresponding to vertex i
    }
*/
void Halfedge_Mesh::bevel_face_positions(const std::vector<Vec3>& start_positions,
                                         Halfedge_Mesh::FaceRef face, float tangent_offset,
                                         float normal_offset) {

    if(flip_orientation) normal_offset = -normal_offset;
    std::vector<HalfedgeRef> new_halfedges;
    auto h = face->halfedge();
    do {
        new_halfedges.push_back(h);
        h = h->next();
    } while(h != face->halfedge());

    Vec3 curr_normal = face->normal();
    Vec3 curr_center = face->center();

    Vec3 norm_vector = curr_normal*normal_offset;
    for(size_t i = 0; i < new_halfedges.size(); i++)
    {
        Vec3 start_pos = start_positions[i];
        Vec3 tangent_vector = start_pos-curr_center;
        new_halfedges[i]->vertex()->pos = start_pos-norm_vector + (tangent_vector.unit())*tangent_offset;
    }

}

/*
    Splits all non-triangular faces into triangles.
*/
void Halfedge_Mesh::triangulate() {
    // Create stack of original faces
    std::vector<FaceRef> original_faces;
    for(FaceRef f = faces_begin(); f != faces_end(); f++) {
        original_faces.push_back(f);
    }

    // Loop through faces and create triangles in any faces that are non-triangular and non-boundary
    while (!original_faces.empty()) {
        FaceRef f0 = original_faces.back();
        HalfedgeRef hStart = f0->halfedge();
        VertexRef vStart = hStart->vertex();
        HalfedgeRef hIterator = hStart->next();
        HalfedgeRef h_outgoing;
        HalfedgeRef h_incoming = hStart;
        if ((hIterator->next()->next() != hStart) && !(f0->is_boundary())) {
            while (hIterator->next()->next() != hStart) {
                HalfedgeRef hNext = hIterator->next();
                h_outgoing = new_halfedge();
                EdgeRef e = new_edge();
                e->halfedge() = h_outgoing;
                FaceRef f = new_face();
                f->halfedge() = h_outgoing;
                
                h_outgoing->face() = f;
                h_outgoing->next() = h_incoming;
                h_outgoing->edge() = e;
                h_outgoing->vertex() = hNext->vertex();

                h_incoming->face() = f;
                h_incoming = new_halfedge();
                h_incoming->next() = hNext;
                h_incoming->vertex() = vStart;
                h_incoming->twin() = h_outgoing;
                h_incoming->edge() = e;

                h_outgoing->twin() = h_incoming;

                hIterator->next() = h_outgoing;
                hIterator->face() = f;
                hIterator = hNext;
            }
            hIterator->next()->next() = h_incoming;
            h_incoming->face() = f0;
            f0->halfedge() = h_incoming;
        }
        original_faces.pop_back();
    }
}

/* Note on the quad subdivision process:

        Unlike the local mesh operations (like bevel or edge flip), we will perform
        subdivision by splitting *all* faces into quads "simultaneously."  Rather
        than operating directly on the halfedge data structure (which as you've
        seen is quite difficult to maintain!) we are going to do something a bit nicer:
           1. Create a raw list of vertex positions and faces (rather than a full-
              blown halfedge mesh).
           2. Build a new halfedge mesh from these lists, replacing the old one.
        Sometimes rebuilding a data structure from scratch is simpler (and even
        more efficient) than incrementally modifying the existing one.  These steps are
        detailed below.

  Step I: Compute the vertex positions for the subdivided mesh.
        Here we're going to do something a little bit strange: since we will
        have one vertex in the subdivided mesh for each vertex, edge, and face in
        the original mesh, we can nicely store the new vertex *positions* as
        attributes on vertices, edges, and faces of the original mesh. These positions
        can then be conveniently copied into the new, subdivided mesh.
        This is what you will implement in linear_subdivide_positions() and
        catmullclark_subdivide_positions().

  Steps II-IV are provided (see Halfedge_Mesh::subdivide()), but are still detailed
  here:

  Step II: Assign a unique index (starting at 0) to each vertex, edge, and
        face in the original mesh. These indices will be the indices of the
        vertices in the new (subdivided mesh).  They do not have to be assigned
        in any particular order, so long as no index is shared by more than one
        mesh element, and the total number of indices is equal to V+E+F, i.e.,
        the total number of vertices plus edges plus faces in the original mesh.
        Basically we just need a one-to-one mapping between original mesh elements
        and subdivided mesh vertices.

  Step III: Build a list of quads in the new (subdivided) mesh, as tuples of
        the element indices defined above. In other words, each new quad should be
        of the form (i,j,k,l), where i,j,k and l are four of the indices stored on
        our original mesh elements.  Note that it is essential to get the orientation
        right here: (i,j,k,l) is not the same as (l,k,j,i).  Indices of new faces
        should circulate in the same direction as old faces (think about the right-hand
        rule).

  Step IV: Pass the list of vertices and quads to a routine that clears
        the internal data for this halfedge mesh, and builds new halfedge data from
        scratch, using the two lists.
*/

/*
    Compute new vertex positions for a mesh that splits each polygon
    into quads (by inserting a vertex at the face midpoint and each
    of the edge midpoints).  The new vertex positions will be stored
    in the members Vertex::new_pos, Edge::new_pos, and
    Face::new_pos.  The values of the positions are based on
    simple linear interpolation, e.g., the edge midpoints and face
    centroids.
*/
void Halfedge_Mesh::linear_subdivide_positions() {

    // For each vertex, assign Vertex::new_pos to
    // its original position, Vertex::pos.

    // For each edge, assign the midpoint of the two original
    // positions to Edge::new_pos.

    // For each face, assign the centroid (i.e., arithmetic mean)
    // of the original vertex positions to Face::new_pos. Note
    // that in general, NOT all faces will be triangles!
}

/*
    Compute new vertex positions for a mesh that splits each polygon
    into quads (by inserting a vertex at the face midpoint and each
    of the edge midpoints).  The new vertex positions will be stored
    in the members Vertex::new_pos, Edge::new_pos, and
    Face::new_pos.  The values of the positions are based on
    the Catmull-Clark rules for subdivision.

    Note: this will only be called on meshes without boundary
*/
void Halfedge_Mesh::catmullclark_subdivide_positions() {

    // The implementation for this routine should be
    // a lot like Halfedge_Mesh:linear_subdivide_positions:(),
    // except that the calculation of the positions themsevles is
    // slightly more involved, using the Catmull-Clark subdivision
    // rules. (These rules are outlined in the Developer Manual.)

    // Faces

    // Edges

    // Vertices
}

/*
        This routine should increase the number of triangles in the mesh
        using Loop subdivision. Note: this is will only be called on triangle meshes.
*/
void Halfedge_Mesh::loop_subdivide() {

    // Compute new positions for all the vertices in the input mesh, using
    // the Loop subdivision rule, and store them in Vertex::new_pos.
    // -> At this point, we also want to mark each vertex as being a vertex of the
    //    original mesh. Use Vertex::is_new for this.
    // -> Next, compute the updated vertex positions associated with edges, and
    //    store it in Edge::new_pos.
    // -> Next, we're going to split every edge in the mesh, in any order.  For
    //    future reference, we're also going to store some information about which
    //    subdivided edges come from splitting an edge in the original mesh, and
    //    which edges are new, by setting the flat Edge::is_new. Note that in this
    //    loop, we only want to iterate over edges of the original mesh.
    //    Otherwise, we'll end up splitting edges that we just split (and the
    //    loop will never end!)
    // -> Now flip any new edge that connects an old and new vertex.
    // -> Finally, copy the new vertex positions into final Vertex::pos.

    // Each vertex and edge of the original surface can be associated with a
    // vertex in the new (subdivided) surface.
    // Therefore, our strategy for computing the subdivided vertex locations is to
    // *first* compute the new positions
    // using the connectivity of the original (coarse) mesh; navigating this mesh
    // will be much easier than navigating
    // the new subdivided (fine) mesh, which has more elements to traverse.  We
    // will then assign vertex positions in
    // the new mesh based on the values we computed for the original mesh.

    // Compute updated positions for all the vertices in the original mesh, using
    // the Loop subdivision rule.

    // Next, compute the updated vertex positions associated with edges.

    // Next, we're going to split every edge in the mesh, in any order. For
    // future reference, we're also going to store some information about which
    // subdivided edges come from splitting an edge in the original mesh, and
    // which edges are new.
    // In this loop, we only want to iterate over edges of the original
    // mesh---otherwise, we'll end up splitting edges that we just split (and
    // the loop will never end!)

    // Finally, flip any new edge that connects an old and new vertex.

    // Copy the updated vertex positions to the subdivided mesh.
}

/*
    Isotropic remeshing. Note that this function returns success in a similar
    manner to the local operations, except with only a boolean value.
    (e.g. you may want to return false if this is not a triangle mesh)
*/
bool Halfedge_Mesh::isotropic_remesh() {

    // Compute the mean edge length.
    // Repeat the four main steps for 5 or 6 iterations
    // -> Split edges much longer than the target length (being careful about
    //    how the loop is written!)
    // -> Collapse edges much shorter than the target length.  Here we need to
    //    be EXTRA careful about advancing the loop, because many edges may have
    //    been destroyed by a collapse (which ones?)
    // -> Now flip each edge if it improves vertex degree
    // -> Finally, apply some tangential smoothing to the vertex positions

    // Note: if you erase elements in a local operation, they will not be actually deleted
    // until do_erase or validate are called. This is to facilitate checking
    // for dangling references to elements that will be erased.
    // The rest of the codebase will automatically call validate() after each op,
    // but here simply calling collapse_edge() will not erase the elements.
    // You should use collapse_edge_erase() instead for the desired behavior.

    return false;
}

/* Helper type for quadric simplification */
struct Edge_Record {
    Halfedge_Mesh::EdgeRef edge;
    Vec3 optimal;
    Mat4 edgeQuadric;
    float cost;
    Edge_Record() {
    }
    Edge_Record(std::unordered_map<Halfedge_Mesh::VertexRef, Mat4>& vertex_quadrics,
                Halfedge_Mesh::EdgeRef e)
        : edge(e) {

        // Compute the combined quadric from the edge endpoints.
        // -> Build the 3x3 linear system whose solution minimizes the quadric error
        //    associated with these two endpoints.
        // -> Use this system to solve for the optimal position, and store it in
        //    Edge_Record::optimal.
        // -> Also store the cost associated with collapsing this edge in
        //    Edge_Record::cost.
        edge = e;
        Halfedge_Mesh::VertexRef v1 = e->halfedge()->vertex();
        Halfedge_Mesh::VertexRef v2 = e->halfedge()->twin()->vertex();
        edgeQuadric = vertex_quadrics[v1] + vertex_quadrics[v2];
        Vec4 b = Vec4(-edgeQuadric[3][0], -edgeQuadric[3][1], -edgeQuadric[3][2], 1.0);
        Mat4 A = vertex_quadrics[v1] + vertex_quadrics[v2];
        A[3][0] = 0;
        A[3][1] = 0;
        A[3][2] = 0;
        A[3][0] = 0;
        A[1][3] = 0;
        A[2][3] = 0;
        A[3][3] = 1;
        if (A.det() > 0.1) {
            Vec4 x = A.inverse() * b;
            optimal = Vec3(x[0], x[1], x[2]);
        } else {
            optimal = Halfedge_Mesh::center_of(e);
        }
        Vec4 optimal_4 = Vec4(optimal[0], optimal[1], optimal[2], 1.0);
        cost = dot(optimal_4, edgeQuadric*optimal_4);
    }
};

/* Comparison operator for Edge_Records so std::set will properly order them */
bool operator<(const Edge_Record& r1, const Edge_Record& r2) {
    if(r1.cost != r2.cost) {
        return r1.cost < r2.cost;
    }
    Halfedge_Mesh::EdgeRef e1 = r1.edge;
    Halfedge_Mesh::EdgeRef e2 = r2.edge;
    return &*e1 < &*e2;
}

/** Helper type for quadric simplification
 *
 * A PQueue is a minimum-priority queue that
 * allows elements to be both inserted and removed from the
 * queue.  Together, one can easily change the priority of
 * an item by removing it, and re-inserting the same item
 * but with a different priority.  A priority queue, for
 * those who don't remember or haven't seen it before, is a
 * data structure that always keeps track of the item with
 * the smallest priority or "score," even as new elements
 * are inserted and removed.  Priority queues are often an
 * essential component of greedy algorithms, where one wants
 * to iteratively operate on the current "best" element.
 *
 * PQueue is templated on the type T of the object
 * being queued.  For this reason, T must define a comparison
 * operator of the form
 *
 *    bool operator<( const T& t1, const T& t2 )
 *
 * which returns true if and only if t1 is considered to have a
 * lower priority than t2.
 *
 * Basic use of a PQueue might look
 * something like this:
 *
 *    // initialize an empty queue
 *    PQueue<myItemType> queue;
 *
 *    // add some items (which we assume have been created
 *    // elsewhere, each of which has its priority stored as
 *    // some kind of internal member variable)
 *    queue.insert( item1 );
 *    queue.insert( item2 );
 *    queue.insert( item3 );
 *
 *    // get the highest priority item currently in the queue
 *    myItemType highestPriorityItem = queue.top();
 *
 *    // remove the highest priority item, automatically
 *    // promoting the next-highest priority item to the top
 *    queue.pop();
 *
 *    myItemType nextHighestPriorityItem = queue.top();
 *
 *    // Etc.
 *
 *    // We can also remove an item, making sure it is no
 *    // longer in the queue (note that this item may already
 *    // have been removed, if it was the 1st or 2nd-highest
 *    // priority item!)
 *    queue.remove( item2 );
 *
 */
template<class T> struct PQueue {
    void insert(const T& item) {
        queue.insert(item);
    }
    void remove(const T& item) {
        if(queue.find(item) != queue.end()) {
            queue.erase(item);
        }
    }
    const T& top(void) const {
        return *(queue.begin());
    }
    void pop(void) {
        queue.erase(queue.begin());
    }
    size_t size() {
        return queue.size();
    }

    std::set<T> queue;
};

/*
    Mesh simplification. Note that this function returns success in a similar
    manner to the local operations, except with only a boolean value.
    (e.g. you may want to return false if you can't simplify the mesh any
    further without destroying it.)
*/
bool Halfedge_Mesh::simplify() {

    std::unordered_map<VertexRef, Mat4> vertex_quadrics;
    std::unordered_map<FaceRef, Mat4> face_quadrics;
    std::unordered_map<EdgeRef, Edge_Record> edge_records;
    PQueue<Edge_Record> edge_queue;

    // Compute initial quadrics for each face by simply writing the plane equation
    // for the face in homogeneous coordinates. These quadrics should be stored
    // in face_quadrics
    // -> Compute an initial quadric for each vertex as the sum of the quadrics
    //    associated with the incident faces, storing it in vertex_quadrics
    // -> Build a priority queue of edges according to their quadric error cost,
    //    i.e., by building an Edge_Record for each edge and sticking it in the
    //    queue. You may want to use the above PQueue<Edge_Record> for this.
    // -> Until we reach the target edge budget, collapse the best edge. Remember
    //    to remove from the queue any edge that touches the collapsing edge
    //    BEFORE it gets collapsed, and add back into the queue any edge touching
    //    the collapsed vertex AFTER it's been collapsed. Also remember to assign
    //    a quadric to the collapsed vertex, and to pop the collapsed edge off the
    //    top of the queue.

    // Note: if you erase elements in a local operation, they will not be actually deleted
    // until do_erase or validate are called. This is to facilitate checking
    // for dangling references to elements that will be erased.
    // The rest of the codebase will automatically call validate() after each op,
    // but here simply calling collapse_edge() will not erase the elements.
    // You should use collapse_edge_erase() instead for the desired behavior.

    // Compute quadrics for all faces
    int initialFaceCount = 0;
    for(FaceRef f = faces_begin(); f != faces_end(); f++) {
        if (f->is_boundary()) { // Set no contribution to sum for boundary faces 
            face_quadrics[f] = Mat4::Zero;
        } else {
            if (f->halfedge()->next()->next()->next() != f->halfedge()) {
                return false;
            }
            VertexRef v1 = f->halfedge()->vertex();
            Vec3 N = normal_of(f);
            Vec4 plane_vector = Vec4(N[0], N[1], N[2], -dot(N, (v1->pos)));
            Mat4 quadric = outer(plane_vector, plane_vector);
            face_quadrics[f] = quadric;
            initialFaceCount = initialFaceCount + 1;
        }
    }

    // Compute quadrics for all vertices
    for(VertexRef v = vertices_begin(); v != vertices_end(); v++) {
        Mat4 quadric = Mat4::Zero;
        HalfedgeRef hStart = v->halfedge();
        HalfedgeRef hIterator = hStart;
        while (hIterator->twin()->next() != hStart) {
            quadric = quadric + face_quadrics[hIterator->face()];
            hIterator = hIterator->twin()->next();
        }
        vertex_quadrics[v] = quadric;
    }
    
    // Initialize queue of edges to be removed
    for(EdgeRef e = edges_begin(); e != edges_end(); e++) {
        Edge_Record record = Edge_Record(vertex_quadrics, e);
        edge_records[e] = record;
        edge_queue.insert(record);
    }

    // Remove edges until desired number of faces while
    // adjusting quadrics, edge records, and queue as appropriate
    int currentFaceCount = initialFaceCount;
    VertexRef collapsed_edge;
    while (currentFaceCount > initialFaceCount/4){
        if (currentFaceCount <= 2) {
            return false;
        }
        if (edge_queue.size() == 0) {
            for(EdgeRef e = edges_begin(); e != edges_end(); e++) {
                if ((e->halfedge()->vertex()->degree() > 2) &&
                   (e->halfedge()->twin()->vertex()->degree() > 2) && 
                   (e->halfedge()->twin()->vertex() != e->halfedge()->vertex())) {
                    Edge_Record record = Edge_Record(vertex_quadrics, e);
                    edge_records[e] = record;
                    edge_queue.insert(record);
                }
            }
            // If there are no edges with vertices with degrees > 2 and different end points return
            if (edge_queue.size() == 0) { 
                return false;
            }
        }
        Edge_Record bestEdge = edge_queue.top();
        EdgeRef chosen_e = bestEdge.edge;

        if (chosen_e->on_boundary()){
            currentFaceCount = currentFaceCount - 1;
        } else {
            currentFaceCount = currentFaceCount - 2;
        }
        Mat4 new_quadric = bestEdge.edgeQuadric;

        // Remove adjacent edges from edge_queue and from records
        EdgeRef e;
        Edge_Record record;
        HalfedgeRef hStart = chosen_e->halfedge();
        HalfedgeRef hIterator = hStart;
        while (hIterator->twin()->next() != hStart) {
            e = hIterator->edge();
            record = edge_records[e];
            edge_queue.remove(record);
            hIterator = hIterator->twin()->next();
        }
        e = hIterator->edge();
        record = edge_records[e];
        edge_queue.remove(record);

        hStart = chosen_e->halfedge()->twin();
        hIterator = hStart;
        while (hIterator->twin()->next() != hStart) {
            e = hIterator->edge();
            record = edge_records[e];
            edge_queue.remove(record);
            hIterator = hIterator->twin()->next();
        }
        e = hIterator->edge();
        record = edge_records[e];
        edge_queue.remove(record);
        std::optional<VertexRef> collapsed_edge_opt = collapse_edge_erase(chosen_e);
        if (collapsed_edge_opt) {
            collapsed_edge = *collapsed_edge_opt;
            vertex_quadrics[collapsed_edge] = new_quadric;
            collapsed_edge->pos = bestEdge.optimal;
            hStart = collapsed_edge->halfedge();
            hIterator = hStart;
            while (hIterator->twin()->next() != hStart) {
                e = hIterator->edge();
                record = Edge_Record(vertex_quadrics, e);
                edge_records[e] = record;
                edge_queue.insert(record);
                hIterator = hIterator->twin()->next();
            }
            e = hIterator->edge();
            record = Edge_Record(vertex_quadrics, e);
            edge_records[e] = record;
            edge_queue.insert(record);
        } else {  // reset if collapse_edge fails
            hStart = chosen_e->halfedge();
            hIterator = hStart;
            while (hIterator->twin()->next() != hStart) {
                e = hIterator->edge();
                if ((e != chosen_e) && (e->halfedge()->vertex()->degree() > 2) &&
                   (e->halfedge()->twin()->vertex()->degree() > 2) && 
                   (e->halfedge()->twin()->vertex() != e->halfedge()->vertex())) {
                    record = Edge_Record(vertex_quadrics, e);
                    edge_records[e] = record;
                    edge_queue.insert(record);
                }
                hIterator = hIterator->twin()->next();
            }
            e = hIterator->edge();
            if ((e != chosen_e) && (e->halfedge()->vertex()->degree() > 2) &&
               (e->halfedge()->twin()->vertex()->degree() > 2) && 
               (e->halfedge()->twin()->vertex() != e->halfedge()->vertex())) {
                record = Edge_Record(vertex_quadrics, e);
                edge_records[e] = record;
                edge_queue.insert(record);
            }
            hStart = chosen_e->halfedge()->twin();
            hIterator = hStart;
            while (hIterator->twin()->next() != hStart) {
                e = hIterator->edge();
                if ((e != chosen_e) && (e->halfedge()->vertex()->degree() > 2) &&
                   (e->halfedge()->twin()->vertex()->degree() > 2) && 
                   (e->halfedge()->twin()->vertex() != e->halfedge()->vertex())) {
                    record = Edge_Record(vertex_quadrics, e);
                    edge_records[e] = record;
                    edge_queue.insert(record);
                }
                hIterator = hIterator->twin()->next();
            }
            e = hIterator->edge();
            if ((e != chosen_e) && (e->halfedge()->vertex()->degree() > 2) &&
               (e->halfedge()->twin()->vertex()->degree() > 2) && 
               (e->halfedge()->twin()->vertex() != e->halfedge()->vertex())) {
                record = Edge_Record(vertex_quadrics, e);
                edge_records[e] = record;
                edge_queue.insert(record);
            }
            if (chosen_e->on_boundary()){
                currentFaceCount = currentFaceCount + 1;
            } else {
                currentFaceCount = currentFaceCount + 2;
            }
        }
    }
    return true;
}
