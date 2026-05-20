# Cork User Guide

Welcome to the modernized **Cork Boolean Library**. This guide provides comprehensive information on how to integrate and use Cork in your projects.

---

## Table of Contents
1. [Introduction](#introduction)
2. [C++ API Reference](#c-api-reference)
   - [Basic Structures](#basic-structures)
   - [Boolean Operations](#boolean-operations)
   - [Error Handling](#error-handling)
3. [Command Line Interface (CLI)](#command-line-interface-cli)
4. [Mesh Requirements](#mesh-requirements)
5. [Integration Guide](#integration-guide)

---

## Introduction

Cork is a high-performance library designed for performing **robust Boolean operations** on triangle meshes. It utilizes **GMP-based exact arithmetic**, ensuring that operations like union, intersection, and difference are geometrically correct and watertight, even in degenerate cases.

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

### Requirements
- **C++ Standard:** C++20 or higher.
- **Dependencies:** GMP (GNU Multi-Precision Library) is required for exact arithmetic.
