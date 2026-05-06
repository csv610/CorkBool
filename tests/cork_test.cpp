#include <cmath>
#include <cassert>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <memory>
#include "cork.h"
#include "files.h"

void test_single_triangle();
void test_tetrahedron();
void test_cube();
void test_sample_files();

struct Vertex {
    float x, y, z;
};

void add_vertex(CorkTriMesh* mesh, Vertex v) {
    uint idx = mesh->n_vertices;
    mesh->vertices[3*idx+0] = v.x;
    mesh->vertices[3*idx+1] = v.y;
    mesh->vertices[3*idx+2] = v.z;
    mesh->n_vertices++;
}

void add_triangle(CorkTriMesh* mesh, uint a, uint b, uint c) {
    uint idx = mesh->n_triangles;
    mesh->triangles[3*idx+0] = a;
    mesh->triangles[3*idx+1] = b;
    mesh->triangles[3*idx+2] = c;
    mesh->n_triangles++;
}

CorkTriMesh create_mesh(uint n_vertices, uint n_triangles) {
    CorkTriMesh mesh;
    mesh.n_vertices = 0;
    mesh.n_triangles = 0;
    mesh.vertices = std::make_unique<float[]>(n_vertices * 3).release();
    mesh.triangles = std::make_unique<uint[]>(n_triangles * 3).release();
    return mesh;
}

void check_solid(CorkTriMesh mesh, bool expected, const char* test_name) {
    bool result = isSolid(mesh);
    if (result != expected) {
        std::cerr << "FAIL: " << test_name << " - expected " 
                  << (expected ? "solid" : "not solid") 
                  << " but got " << (result ? "solid" : "not solid") 
                  << std::endl;
        std::exit(1);
    }
    std::cout << "PASS: " << test_name << std::endl;
}

void test_single_triangle() {
    CorkTriMesh mesh = create_mesh(3, 1);
    add_vertex(&mesh, {0, 0, 0});
    add_vertex(&mesh, {1, 0, 0});
    add_vertex(&mesh, {0, 1, 0});
    add_triangle(&mesh, 0, 1, 2);
    check_solid(mesh, false, "test_single_triangle");
    delete[] mesh.vertices;
    delete[] mesh.triangles;
}

void test_tetrahedron() {
    CorkTriMesh mesh = create_mesh(4, 4);
    add_vertex(&mesh, {1, 0, 0});
    add_vertex(&mesh, {-1, 0, 0});
    add_vertex(&mesh, {0, 1, 0});
    add_vertex(&mesh, {0, 0, 1});
    add_triangle(&mesh, 0, 2, 3);
    add_triangle(&mesh, 0, 3, 1);
    add_triangle(&mesh, 0, 1, 2);
    add_triangle(&mesh, 1, 3, 2);
    check_solid(mesh, true, "test_tetrahedron");
    delete[] mesh.vertices;
    delete[] mesh.triangles;
}

void test_cube() {
    CorkTriMesh mesh = create_mesh(8, 12);
    float s = 1.0f;
    add_vertex(&mesh, {-s, -s, -s});
    add_vertex(&mesh, { s, -s, -s});
    add_vertex(&mesh, { s,  s, -s});
    add_vertex(&mesh, {-s,  s, -s});
    add_vertex(&mesh, {-s, -s,  s});
    add_vertex(&mesh, { s, -s,  s});
    add_vertex(&mesh, { s,  s,  s});
    add_vertex(&mesh, {-s,  s,  s});
    
    add_triangle(&mesh, 0, 1, 2);
    add_triangle(&mesh, 0, 2, 3);
    add_triangle(&mesh, 4, 6, 5);
    add_triangle(&mesh, 4, 7, 6);
    add_triangle(&mesh, 0, 4, 5);
    add_triangle(&mesh, 0, 5, 1);
    add_triangle(&mesh, 2, 6, 7);
    add_triangle(&mesh, 2, 7, 3);
    add_triangle(&mesh, 0, 3, 7);
    add_triangle(&mesh, 0, 7, 4);
    add_triangle(&mesh, 1, 5, 6);
    add_triangle(&mesh, 1, 6, 2);
    
    check_solid(mesh, true, "test_cube");
    delete[] mesh.vertices;
    delete[] mesh.triangles;
}

void test_sample_files() {
    Files::FileMesh meshA, meshB;
    
    if (Files::readTriMesh("/Users/csv610/Projects/Mesh/Cork/samples/ballA.off", &meshA) != 0) {
        std::cerr << "FAIL: test_sample_files - could not load ballA.off" << std::endl;
        std::exit(1);
    }
    if (Files::readTriMesh("/Users/csv610/Projects/Mesh/Cork/samples/ballB.off", &meshB) != 0) {
        std::cerr << "FAIL: test_sample_files - could not load ballB.off" << std::endl;
        std::exit(1);
    }
    
    CorkTriMesh in0, in1;
    in0.n_vertices = meshA.vertices.size();
    in0.n_triangles = meshA.triangles.size();
    in0.triangles = std::make_unique<uint[]>(in0.n_triangles * 3).release();
    in0.vertices = std::make_unique<float[]>(in0.n_vertices * 3).release();
    
    for (uint i = 0; i < in0.n_triangles; i++) {
        in0.triangles[3*i+0] = meshA.triangles[i].a;
        in0.triangles[3*i+1] = meshA.triangles[i].b;
        in0.triangles[3*i+2] = meshA.triangles[i].c;
    }
    for (uint i = 0; i < in0.n_vertices; i++) {
        in0.vertices[3*i+0] = meshA.vertices[i].pos.x;
        in0.vertices[3*i+1] = meshA.vertices[i].pos.y;
        in0.vertices[3*i+2] = meshA.vertices[i].pos.z;
    }
    
    in1.n_vertices = meshB.vertices.size();
    in1.n_triangles = meshB.triangles.size();
    in1.triangles = std::make_unique<uint[]>(in1.n_triangles * 3).release();
    in1.vertices = std::make_unique<float[]>(in1.n_vertices * 3).release();
    
    for (uint i = 0; i < in1.n_triangles; i++) {
        in1.triangles[3*i+0] = meshB.triangles[i].a;
        in1.triangles[3*i+1] = meshB.triangles[i].b;
        in1.triangles[3*i+2] = meshB.triangles[i].c;
    }
    for (uint i = 0; i < in1.n_vertices; i++) {
        in1.vertices[3*i+0] = meshB.vertices[i].pos.x;
        in1.vertices[3*i+1] = meshB.vertices[i].pos.y;
        in1.vertices[3*i+2] = meshB.vertices[i].pos.z;
    }
    
    bool solidA = isSolid(in0);
    bool solidB = isSolid(in1);
    
    if (!solidA) {
        std::cerr << "FAIL: test_sample_files - ballA is not solid" << std::endl;
        std::exit(1);
    }
    if (!solidB) {
        std::cerr << "FAIL: test_sample_files - ballB is not solid" << std::endl;
        std::exit(1);
    }
    
    std::cout << "PASS: test_sample_files" << std::endl;
    
    delete[] in0.vertices;
    delete[] in0.triangles;
    delete[] in1.vertices;
    delete[] in1.triangles;
}

int main() {
    std::cout << "Running unit tests..." << std::endl;
    
    test_single_triangle();
    test_tetrahedron();
    test_cube();
    test_sample_files();
    
    std::cout << "All tests passed!" << std::endl;
    return 0;
}