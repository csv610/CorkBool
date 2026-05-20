# Cork Boolean Library (Professional Edition)

A high-performance C++20 library for performing Boolean operations on triangle meshes.

## Key Features

- **Exact Arithmetic:** Uses GMP-based exact arithmetic for robust Boolean operations.
- **Modern C++ API:** RAII-compliant, namespaced, and exception-safe.
- **Robustness:** Handles non-manifold and self-intersecting meshes with clear error reporting.
- **Command Line Interface:** Intuitive tool for batch processing.

## Requirements

- CMake 3.16+
- C++20 Compiler (GCC 10+, Clang 10+, MSVC 2019+)
- GMP 6.0+

## Building

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

## Usage

### C++ API

```cpp
#include <cork/cork.h>

try {
    cork::CorkTriMesh meshA = ...;
    cork::CorkTriMesh meshB = ...;
    
    // Compute result
    auto result = cork::computeUnion(meshA, meshB);
    
    // result is automatically cleaned up via RAII
} catch (const cork::CorkException& e) {
    std::cerr << "Cork error: " << e.what() << std::endl;
}
```

### CLI

```bash
./cork_cli -union inputA.off inputB.off output.off
```

## License

Licensed under LGPL v3. See COPYRIGHT for details.
