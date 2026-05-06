#include <cmath>
#include <cassert>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <memory>
#include "cork.h"
#include "files.h"
#include "shortVec.h"

int tests_passed = 0;
int tests_failed = 0;

void assert_true(bool condition, const char* test_name) {
    if (!condition) {
        std::cerr << "FAIL: " << test_name << std::endl;
        tests_failed++;
    } else {
        std::cout << "PASS: " << test_name << std::endl;
        tests_passed++;
    }
}

template<typename T, typename U>
void assert_eq(T expected, U actual, const char* test_name) {
    if (expected != (T)actual) {
        std::cerr << "FAIL: " << test_name << " - expected " << expected << " got " << actual << std::endl;
        tests_failed++;
    } else {
        std::cout << "PASS: " << test_name << std::endl;
        tests_passed++;
    }
}

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

void free_mesh(CorkTriMesh* mesh) {
    delete[] mesh->vertices;
    delete[] mesh->triangles;
    mesh->n_vertices = 0;
    mesh->n_triangles = 0;
}

CorkTriMesh create_tetrahedron() {
    CorkTriMesh mesh = create_mesh(4, 4);
    add_vertex(&mesh, {1, 0, 0});
    add_vertex(&mesh, {-1, 0, 0});
    add_vertex(&mesh, {0, 1, 0});
    add_vertex(&mesh, {0, 0, 1});
    add_triangle(&mesh, 0, 2, 3);
    add_triangle(&mesh, 0, 3, 1);
    add_triangle(&mesh, 0, 1, 2);
    add_triangle(&mesh, 1, 3, 2);
    return mesh;
}

CorkTriMesh create_cube() {
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
    return mesh;
}

void test_empty_mesh() {
    CorkTriMesh mesh = create_mesh(0, 0);
    assert_eq(0, (int)mesh.n_vertices, "empty mesh has 0 vertices");
    assert_eq(0, (int)mesh.n_triangles, "empty mesh has 0 triangles");
    free_mesh(&mesh);
}

void test_single_triangle() {
    CorkTriMesh mesh = create_mesh(3, 1);
    add_vertex(&mesh, {0, 0, 0});
    add_vertex(&mesh, {1, 0, 0});
    add_vertex(&mesh, {0, 1, 0});
    add_triangle(&mesh, 0, 1, 2);
    assert_true(!isSolid(mesh), "single triangle is not solid");
    free_mesh(&mesh);
}

void test_tetrahedron() {
    CorkTriMesh mesh = create_tetrahedron();
    assert_true(isSolid(mesh), "tetrahedron is solid");
    free_mesh(&mesh);
}

void test_cube() {
    CorkTriMesh mesh = create_cube();
    assert_true(isSolid(mesh), "cube is solid");
    free_mesh(&mesh);
}

void test_pyramid() {
    CorkTriMesh mesh = create_mesh(5, 6);
    add_vertex(&mesh, {0, 1, 0});
    add_vertex(&mesh, {-1, 0, -1});
    add_vertex(&mesh, {1, 0, -1});
    add_vertex(&mesh, {1, 0, 1});
    add_vertex(&mesh, {-1, 0, 1});
    
    add_triangle(&mesh, 0, 2, 1);
    add_triangle(&mesh, 0, 3, 2);
    add_triangle(&mesh, 0, 4, 3);
    add_triangle(&mesh, 0, 1, 4);
    add_triangle(&mesh, 1, 2, 3);
    add_triangle(&mesh, 1, 3, 4);
    
    assert_true(isSolid(mesh), "pyramid is solid");
    free_mesh(&mesh);
}

void test_octahedron() {
    CorkTriMesh mesh = create_mesh(6, 8);
    add_vertex(&mesh, {1, 0, 0});
    add_vertex(&mesh, {-1, 0, 0});
    add_vertex(&mesh, {0, 1, 0});
    add_vertex(&mesh, {0, -1, 0});
    add_vertex(&mesh, {0, 0, 1});
    add_vertex(&mesh, {0, 0, -1});
    
    add_triangle(&mesh, 0, 2, 4);
    add_triangle(&mesh, 0, 4, 3);
    add_triangle(&mesh, 0, 3, 5);
    add_triangle(&mesh, 0, 5, 2);
    add_triangle(&mesh, 1, 2, 5);
    add_triangle(&mesh, 1, 5, 3);
    add_triangle(&mesh, 1, 3, 4);
    add_triangle(&mesh, 1, 4, 2);
    
    assert_true(!isSolid(mesh), "open octahedron is not solid");
    free_mesh(&mesh);
}

void test_degenerate_triangle() {
    CorkTriMesh mesh = create_mesh(3, 1);
    add_vertex(&mesh, {0, 0, 0});
    add_vertex(&mesh, {1, 0, 0});
    add_vertex(&mesh, {1, 0, 0});
    add_triangle(&mesh, 0, 1, 2);
    assert_true(!isSolid(mesh), "degenerate triangle (zero area) is not solid");
    free_mesh(&mesh);
}

void test_duplicate_triangle() {
    CorkTriMesh mesh = create_mesh(3, 2);
    add_vertex(&mesh, {0, 0, 0});
    add_vertex(&mesh, {1, 0, 0});
    add_vertex(&mesh, {0, 1, 0});
    add_triangle(&mesh, 0, 1, 2);
    add_triangle(&mesh, 0, 1, 2);
    assert_true(!isSolid(mesh), "duplicate triangle is not solid");
    free_mesh(&mesh);
}

void test_non_manifold() {
    CorkTriMesh mesh = create_mesh(4, 5);
    add_vertex(&mesh, {0, 0, 0});
    add_vertex(&mesh, {1, 0, 0});
    add_vertex(&mesh, {0, 1, 0});
    add_vertex(&mesh, {0, 0, 1});
    
    add_triangle(&mesh, 0, 1, 2);
    add_triangle(&mesh, 0, 1, 3);
    add_triangle(&mesh, 0, 2, 3);
    add_triangle(&mesh, 1, 2, 3);
    add_triangle(&mesh, 0, 2, 1);
    
    assert_true(!isSolid(mesh), "non-manifold is not solid");
    free_mesh(&mesh);
}

void test_self_intersecting() {
    CorkTriMesh mesh = create_mesh(4, 2);
    add_vertex(&mesh, {0, 0, 0});
    add_vertex(&mesh, {2, 0, 0});
    add_vertex(&mesh, {1, 1, 0});
    add_vertex(&mesh, {1, -1, 0});
    
    add_triangle(&mesh, 0, 1, 2);
    add_triangle(&mesh, 0, 1, 3);
    
    assert_true(!isSolid(mesh), "self-intersecting is not solid");
    free_mesh(&mesh);
}

void test_union_two_cubes() {
    CorkTriMesh cube1 = create_cube();
    CorkTriMesh cube2 = create_mesh(8, 12);
    float offset = 1.5f;
    for (int i = 0; i < 8; i++) {
        Vertex v;
        v.x = cube1.vertices[i * 3] + offset;
        v.y = cube1.vertices[i * 3 + 1];
        v.z = cube1.vertices[i * 3 + 2];
        add_vertex(&cube2, v);
    }
    for (int i = 0; i < 12; i++) {
        add_triangle(&cube2, cube1.triangles[i*3], cube1.triangles[i*3+1], cube1.triangles[i*3+2]);
    }
    
    CorkTriMesh result;
    computeUnion(cube1, cube2, &result);
    
    assert_true(result.n_vertices > 0, "union produces vertices");
    assert_true(result.n_triangles > 0, "union produces triangles");
    
    free_mesh(&cube1);
    free_mesh(&cube2);
    free_mesh(&result);
}

void test_intersection_two_cubes() {
    CorkTriMesh cube1 = create_cube();
    CorkTriMesh cube2 = create_cube();
    
    CorkTriMesh result;
    computeIntersection(cube1, cube2, &result);
    
    assert_true(result.n_vertices > 0, "intersection produces vertices");
    assert_true(result.n_triangles > 0, "intersection produces triangles");
    
    free_mesh(&cube1);
    free_mesh(&cube2);
    free_mesh(&result);
}

void test_difference_two_cubes() {
    CorkTriMesh cube1 = create_cube();
    CorkTriMesh cube2 = create_cube();
    
    CorkTriMesh result;
    computeDifference(cube1, cube2, &result);
    
    free_mesh(&cube1);
    free_mesh(&cube2);
    free_mesh(&result);
}

void test_symmetric_difference() {
    CorkTriMesh cube1 = create_cube();
    CorkTriMesh cube2 = create_cube();
    float offset = 0.5f;
    for (uint i = 0; i < cube2.n_vertices; i++) {
        cube2.vertices[i*3] += offset;
    }
    
    CorkTriMesh result;
    computeSymmetricDifference(cube1, cube2, &result);
    
    assert_true(result.n_vertices > 0, "symmetric difference produces output");
    
    free_mesh(&cube1);
    free_mesh(&cube2);
    free_mesh(&result);
}

void test_resolve_intersections() {
    CorkTriMesh cube1 = create_cube();
    CorkTriMesh cube2 = create_cube();
    float offset = 0.1f;
    for (uint i = 0; i < cube2.n_vertices; i++) {
        cube2.vertices[i*3] += offset;
    }
    
    CorkTriMesh result;
    resolveIntersections(cube1, cube2, &result);
    
    assert_true(result.n_vertices > 0, "resolve produces output");
    
    free_mesh(&cube1);
    free_mesh(&cube2);
    free_mesh(&result);
}

void test_file_read() {
    Files::FileMesh mesh;
    int result = Files::readTriMesh("/Users/csv610/Projects/Mesh/Cork/samples/ballA.off", &mesh);
    assert_eq(0, result, "read ballA.off");
    assert_true(mesh.vertices.size() > 0, "ballA has vertices");
    assert_true(mesh.triangles.size() > 0, "ballA has triangles");
}

void test_file_write() {
    CorkTriMesh mesh = create_cube();
    Files::FileMesh converted;
    converted.vertices.resize(mesh.n_vertices);
    converted.triangles.resize(mesh.n_triangles);
    
    for (uint i = 0; i < mesh.n_vertices; i++) {
        converted.vertices[i].pos.x = mesh.vertices[i*3];
        converted.vertices[i].pos.y = mesh.vertices[i*3+1];
        converted.vertices[i].pos.z = mesh.vertices[i*3+2];
    }
    for (uint i = 0; i < mesh.n_triangles; i++) {
        converted.triangles[i].a = mesh.triangles[i*3];
        converted.triangles[i].b = mesh.triangles[i*3+1];
        converted.triangles[i].c = mesh.triangles[i*3+2];
    }
    
    int result = Files::writeTriMesh("/tmp/test_cube.off", &converted);
    assert_eq(0, result, "write mesh");
    
    Files::FileMesh loaded;
    int read_result = Files::readTriMesh("/tmp/test_cube.off", &loaded);
    assert_eq((int)converted.vertices.size(), (int)loaded.vertices.size(), "read written mesh vertices");
    
    free_mesh(&mesh);
}

void test_free_cork_tri_mesh() {
    CorkTriMesh mesh = create_cube();
    assert_true(mesh.n_vertices > 0, "mesh has vertices before free");
    freeCorkTriMesh(&mesh);
    assert_eq(0, (int)mesh.n_vertices, "mesh vertices cleared");
    assert_eq(0, (int)mesh.n_triangles, "mesh triangles cleared");
}

void test_math_vec3_operations() {
    Vec3d v1(1, 0, 0);
    Vec3d v2(0, 1, 0);
    Vec3d v3 = v1 + v2;
    assert_true(v3.x == 1 && v3.y == 1 && v3.z == 0, "Vec3 addition");
    
    Vec3d v4 = v1 - v2;
    assert_true(v4.x == 1 && v4.y == -1 && v4.z == 0, "Vec3 subtraction");
    
    Vec3d v5 = v1 * 2.0;
    assert_true(v5.x == 2 && v5.y == 0 && v5.z == 0, "Vec3 scalar mult");
    
    double dot_result = dot(v1, v2);
    assert_true(dot_result == 0, "Vec3 dot product");
    
    Vec3d cross_result = cross(v1, v2);
    assert_true(cross_result.z == 1, "Vec3 cross product");
    
    double length = len(v1);
    assert_true(std::abs(length - 1.0) < 0.001, "Vec3 length");
}

void test_short_vec() {
    ShortVec<int, 4> sv(3, 1);
    assert_eq(3, (int)sv.size(), "ShortVec size");
    sv.push_back(2);
    assert_eq(4, (int)sv.size(), "ShortVec after push_back");
    sv.erase(1);
    assert_eq(3, (int)sv.size(), "ShortVec after erase");
}

int main() {
    std::cout << "Running comprehensive unit tests..." << std::endl;
    std::cout << "==========================================" << std::endl;
    
    std::cout << "\n--- Basic mesh tests ---" << std::endl;
    test_empty_mesh();
    test_single_triangle();
    test_tetrahedron();
    test_cube();
    test_pyramid();
    test_octahedron();
    
    std::cout << "\n--- Edge case tests ---" << std::endl;
    test_degenerate_triangle();
    test_duplicate_triangle();
    test_non_manifold();
    test_self_intersecting();
    
    std::cout << "\n--- Boolean operation tests ---" << std::endl;
    test_union_two_cubes();
    test_intersection_two_cubes();
    test_difference_two_cubes();
    test_symmetric_difference();
    test_resolve_intersections();
    
    std::cout << "\n--- File I/O tests ---" << std::endl;
    test_file_read();
    test_file_write();
    
    std::cout << "\n--- Core library tests ---" << std::endl;
    test_free_cork_tri_mesh();
    test_math_vec3_operations();
    test_short_vec();
    
    std::cout << "\n==========================================" << std::endl;
    std::cout << "Tests passed: " << tests_passed << std::endl;
    std::cout << "Tests failed: " << tests_failed << std::endl;
    
    if (tests_failed > 0) {
        std::cerr << "SOME TESTS FAILED!" << std::endl;
        return 1;
    }
    
    std::cout << "All tests passed!" << std::endl;
    return 0;
}