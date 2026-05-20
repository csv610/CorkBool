// +-------------------------------------------------------------------------
// | cork.cpp
// | 
// | Author: Gilbert Bernstein
// +-------------------------------------------------------------------------
#include "cork.h"
#include "mesh.h"
#include "prelude.h"

namespace cork {

struct CorkTriangle;

struct CorkVertex :
    public MinimalVertexData,
    public RemeshVertexData,
    public IsctVertexData,
    public BoolVertexData
{
    void merge(const CorkVertex &v0, const CorkVertex &v1) {
        double a0 = 0.5;
        if(v0.manifold && !v1.manifold) a0 = 0.0;
        if(!v0.manifold && v1.manifold) a0 = 1.0;
        double a1 = 1.0 - a0;
        pos = a0 * v0.pos + a1 * v1.pos;
    }
    void interpolate(const CorkVertex &v0, const CorkVertex &v1) {
        pos = 0.5 * v0.pos + 0.5 * v1.pos;
    }
    
    void isct(IsctVertEdgeTriInput<CorkVertex,CorkTriangle>) {}
    void isct(IsctVertTriTriTriInput<CorkVertex,CorkTriangle>) {}
    
    void isctInterpolate(const CorkVertex &v0, const CorkVertex &v1) {
        double a0 = len(v1.pos - pos);
        double a1 = len(v0.pos - pos);
        double sum = a0 + a1;
        if(sum == 0.0) {
            a0 = a1 = 0.5;
        } else {
            a0 /= sum;
            a1 /= sum;
        }
    }
};

struct CorkTriangle :
    public MinimalTriangleData,
    public RemeshTriangleData,
    public IsctTriangleData,
    public BoolTriangleData
{
    void merge(const CorkTriangle &, const CorkTriangle &) {}
    static void split(CorkTriangle &, CorkTriangle &, const CorkTriangle &) {}
    void move(const CorkTriangle &) {}
    void subdivide(SubdivideTriInput<CorkVertex,CorkTriangle> input) {
        bool_alg_data = input.pt->bool_alg_data;
    }
};

using RawCorkMesh = RawMesh<CorkVertex, CorkTriangle>;
using CorkMesh = Mesh<CorkVertex, CorkTriangle>;

static void corkTriMesh2CorkMesh(const CorkTriMesh& in, CorkMesh* mesh_out) {
    RawCorkMesh raw;
    uint n_tris = in.triangles.size() / 3;
    uint n_verts = in.vertices.size() / 3;

    if(n_verts == 0 || n_tris == 0) {
        throw CorkException(Error::EmptyMesh, "Empty mesh input to Cork routine.");
    }
    
    raw.vertices.resize(n_verts);
    raw.triangles.resize(n_tris);
    
    for(uint i=0; i < n_tris; i++) {
        uint a = in.triangles[3*i+0];
        uint b = in.triangles[3*i+1];
        uint c = in.triangles[3*i+2];
        if (a >= n_verts || b >= n_verts || c >= n_verts) {
             throw CorkException(Error::InvalidVertexReference, "Mesh has out of range vertex reference.");
        }
        raw.triangles[i].a = a;
        raw.triangles[i].b = b;
        raw.triangles[i].c = c;
    }
    
    for(uint i=0; i < n_verts; i++) {
        raw.vertices[i].pos.x = in.vertices[3*i+0];
        raw.vertices[i].pos.y = in.vertices[3*i+1];
        raw.vertices[i].pos.z = in.vertices[3*i+2];
    }
    
    *mesh_out = CorkMesh(raw);
}

static CorkTriMesh corkMesh2CorkTriMesh(CorkMesh* mesh_in) {
    RawCorkMesh raw = mesh_in->raw();
    CorkTriMesh out;
    
    out.triangles.resize(raw.triangles.size() * 3);
    out.vertices.resize(raw.vertices.size() * 3);
    
    for(size_t i=0; i < raw.triangles.size(); i++) {
        out.triangles[3*i+0] = raw.triangles[i].a;
        out.triangles[3*i+1] = raw.triangles[i].b;
        out.triangles[3*i+2] = raw.triangles[i].c;
    }
    
    for(size_t i=0; i < raw.vertices.size(); i++) {
        out.vertices[3*i+0] = static_cast<float>(raw.vertices[i].pos.x);
        out.vertices[3*i+1] = static_cast<float>(raw.vertices[i].pos.y);
        out.vertices[3*i+2] = static_cast<float>(raw.vertices[i].pos.z);
    }
    return out;
}

bool isSolid(const CorkTriMesh& cmesh) {
    CorkMesh mesh;
    try {
        corkTriMesh2CorkMesh(cmesh, &mesh);
    } catch (...) {
        return false;
    }
    
    if(mesh.isSelfIntersecting()) return false;
    if(!mesh.isClosed()) return false;
    
    return true;
}

CorkTriMesh computeUnion(const CorkTriMesh& in0, const CorkTriMesh& in1) {
    CorkMesh cmIn0, cmIn1;
    corkTriMesh2CorkMesh(in0, &cmIn0);
    corkTriMesh2CorkMesh(in1, &cmIn1);
    cmIn0.boolUnion(cmIn1);
    return corkMesh2CorkTriMesh(&cmIn0);
}

CorkTriMesh computeDifference(const CorkTriMesh& in0, const CorkTriMesh& in1) {
    CorkMesh cmIn0, cmIn1;
    corkTriMesh2CorkMesh(in0, &cmIn0);
    corkTriMesh2CorkMesh(in1, &cmIn1);
    cmIn0.boolDiff(cmIn1);
    return corkMesh2CorkTriMesh(&cmIn0);
}

CorkTriMesh computeIntersection(const CorkTriMesh& in0, const CorkTriMesh& in1) {
    CorkMesh cmIn0, cmIn1;
    corkTriMesh2CorkMesh(in0, &cmIn0);
    corkTriMesh2CorkMesh(in1, &cmIn1);
    cmIn0.boolIsct(cmIn1);
    return corkMesh2CorkTriMesh(&cmIn0);
}

CorkTriMesh computeSymmetricDifference(const CorkTriMesh& in0, const CorkTriMesh& in1) {
    CorkMesh cmIn0, cmIn1;
    corkTriMesh2CorkMesh(in0, &cmIn0);
    corkTriMesh2CorkMesh(in1, &cmIn1);
    cmIn0.boolXor(cmIn1);
    return corkMesh2CorkTriMesh(&cmIn0);
}

CorkTriMesh resolveIntersections(const CorkTriMesh& in0, const CorkTriMesh& in1) {
    CorkMesh cmIn0, cmIn1;
    corkTriMesh2CorkMesh(in0, &cmIn0);
    corkTriMesh2CorkMesh(in1, &cmIn1);
    cmIn0.disjointUnion(cmIn1);
    cmIn0.resolveIntersections();
    return corkMesh2CorkTriMesh(&cmIn0);
}

} // namespace cork
