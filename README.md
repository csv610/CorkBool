# Cork Boolean Library (Professional Edition)

A high-performance C++20 library for performing robust Boolean operations on triangle meshes using GMP-based exact arithmetic.

---

## Table of Contents
- [Key Features](#key-features)
- [Requirements](#requirements)
- [Building](#building)
- [C++ API Reference](#c-api-reference)
- [Command Line Interface (CLI)](#command-line-interface-cli)
- [Mesh Requirements](#mesh-requirements)
- [Integration Guide](#integration-guide)
- [License](#license)

---

## Key Features

- **Exact Arithmetic:** Uses GMP-based exact arithmetic for robust Boolean operations.
- **Modern C++ API:** RAII-compliant, namespaced, and exception-safe.
- **Robustness:** Handles non-manifold and self-intersecting meshes with clear error reporting.
- **Performance:** Optimized for speed, with a dedicated benchmarking suite included.

---

## Requirements

- **CMake:** 3.16+
- **C++ Compiler:** GCC 10+, Clang 10+, or MSVC 2019+ (C++20 standard required)
- **Dependencies:** GMP 6.0+ (GNU Multi-Precision Library)

---

## Building

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

To run the unit tests:
```bash
./cork_test
```

To run the benchmarks:
```bash
./cork_bench
```

---

## C++ API Reference

The C++ API is contained within the `cork` namespace and follows modern RAII principles.

### Basic Structures

The primary structure for mesh data is `cork::CorkTriMesh`.

```cpp
namespace cork {
    struct CorkTriMesh {
        std::vector<uint32_t> triangles; // Flat list of vertex indices (3 per triangle)
        std::vector<float> vertices;    // Flat list of coordinates (3 per vertex)
    };
}
```

### Boolean Operations

All operations take two meshes as input and return a new `CorkTriMesh`.

```cpp
#include <cork/cork.h>

// Compute the union of two meshes
cork::CorkTriMesh result = cork::computeUnion(meshA, meshB);

// Compute the intersection
cork::CorkTriMesh result = cork::computeIntersection(meshA, meshB);

// Compute the difference (A - B)
cork::CorkTriMesh result = cork::computeDifference(meshA, meshB);

// Compute the symmetric difference (XOR)
cork::CorkTriMesh result = cork::computeSymmetricDifference(meshA, meshB);

// Resolve intersections without deleting any part of the surface
cork::CorkTriMesh result = cork::resolveIntersections(meshA, meshB);
```

### Error Handling

Cork uses exceptions to report failures. Always wrap your calls in a `try-catch` block.

```cpp
try {
    auto result = cork::computeUnion(meshA, meshB);
} catch (const cork::CorkException& e) {
    std::cerr << "Cork Error: " << e.what() << " (Code: " << (int)e.code() << ")" << std::endl;
}
```

---

## Command Line Interface (CLI)

The `cork_cli` tool allows for batch processing and quick verification.

| Command | Usage | Description |
| :--- | :--- | :--- |
| `-union` | `cork_cli -union in0.off in1.off out.off` | Union of two meshes |
| `-diff` | `cork_cli -diff in0.off in1.off out.off` | Difference (in0 - in1) |
| `-isct` | `cork_cli -isct in0.off in1.off out.off` | Intersection |
| `-xor` | `cork_cli -xor in0.off in1.off out.off` | Symmetric Difference |
| `-solid` | `cork_cli -solid mesh.off` | Check if mesh is watertight/solid |
| `-resolve` | `cork_cli -resolve in0.off in1.off out.off` | Resolve intersections |

---

## Mesh Requirements

For Boolean operations to work reliably, input meshes MUST be **solid**.

1.  **Watertight (Closed):** The mesh must have no holes or open boundaries.
2.  **Non-Self-Intersecting:** The surface must not pass through itself.
3.  **Orientation:** Triangles must follow a **Counter-Clockwise (CCW)** winding order. Clockwise meshes are interpreted as "voids" or the unbounded complement of a solid.

You can verify a mesh using the API call `cork::isSolid(mesh)`.

---

## Integration Guide

### Using CMake

Cork is designed to be easily integrated into CMake projects.

```cmake
find_package(PkgConfig REQUIRED)
pkg_check_modules(GMP REQUIRED gmp gmpxx)

add_subdirectory(path/to/cork)

target_link_libraries(my_app PRIVATE cork_core)
```

---

## License

Licensed under LGPL v3. See the `COPYRIGHT` file for details.
