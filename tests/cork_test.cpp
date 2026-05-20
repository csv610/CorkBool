#include <cmath>
#include <iostream>
#include <vector>
#include <string>
#include "cork.h"
#include "vec.h"

using namespace cork;

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

CorkTriMesh create_cube() {
    CorkTriMesh mesh;
    float s = 1.0f;
    mesh.vertices = {
        -s, -s, -s,  s, -s, -s,  s,  s, -s, -s,  s, -s,
        -s, -s,  s,  s, -s,  s,  s,  s,  s, -s,  s,  s
    };
    mesh.triangles = {
        0, 1, 2, 0, 2, 3, 4, 6, 5, 4, 7, 6,
        0, 4, 5, 0, 5, 1, 2, 6, 7, 2, 7, 3,
        0, 3, 7, 0, 7, 4, 1, 5, 6, 1, 6, 2
    };
    return mesh;
}

void test_solid_cube() {
    assert_true(isSolid(create_cube()), "Cube is solid");
}

void test_boolean_ops() {
    CorkTriMesh c1 = create_cube();
    CorkTriMesh c2 = create_cube();
    for(auto& v : c2.vertices) v += 0.5f;
    
    try {
        auto u = computeUnion(c1, c2);
        assert_true(!u.vertices.empty(), "Union produces result");
        
        auto i = computeIntersection(c1, c2);
        assert_true(!i.vertices.empty(), "Intersection produces result");
    } catch (const std::exception& e) {
        std::cerr << "Exception in boolean ops: " << e.what() << std::endl;
        tests_failed++;
    }
}

int main() {
    std::cout << "Running Refactored Cork Tests..." << std::endl;
    test_solid_cube();
    test_boolean_ops();
    
    std::cout << "Passed: " << tests_passed << ", Failed: " << tests_failed << std::endl;
    return tests_failed > 0 ? 1 : 0;
}
