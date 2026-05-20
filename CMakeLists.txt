cmake_minimum_required(VERSION 3.16)
project(cork VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GMP REQUIRED gmp gmpxx)

# Interface library for headers and dependencies
add_library(cork_interface INTERFACE)
target_include_directories(cork_interface INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/util>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/math>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/isct>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/mesh>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/rawmesh>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/accel>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/file_formats>
    ${GMP_INCLUDE_DIRS}
)
target_link_libraries(cork_interface INTERFACE ${GMP_LIBRARIES})

# Core library
add_library(cork_core STATIC
    src/cork.cpp
    src/util/timer.cpp
    src/util/log.cpp
    src/isct/empty3d.cpp
    src/isct/quantization.cpp
    src/file_formats/files.cpp
    src/file_formats/ifs.cpp
    src/file_formats/off.cpp
    src/isct/triangle.c
)
target_link_libraries(cork_core PUBLIC cork_interface)

# Compiler specific options
if(MSVC)
    target_compile_options(cork_core PRIVATE /W4)
else()
    target_compile_options(cork_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

set_source_files_properties(src/isct/triangle.c PROPERTIES
    COMPILE_OPTIONS "-O2;-DNO_TIMER;-DREDUCED;-DCDT_ONLY;-DTRILIBRARY;-w;-DANSI_DECLARATORS"
)

# CLI
add_executable(cork_cli src/main.cpp)
target_link_libraries(cork_cli PRIVATE cork_core)

# Tools
add_executable(off2obj src/off2obj.cpp)
target_link_libraries(off2obj PRIVATE cork_core)

# Tests
enable_testing()
add_executable(cork_test tests/cork_test.cpp)
target_link_libraries(cork_test PRIVATE cork_core)
add_test(NAME cork_test COMMAND cork_test)

# Benchmarks
add_executable(cork_bench bench/benchmark.cpp)
target_link_libraries(cork_bench PRIVATE cork_core)

# Killer Apps (Proof of Concepts)
add_executable(surgical_planner apps/surgical_planner.cpp)
target_link_libraries(surgical_planner PRIVATE cork_core)

add_executable(infill_generator apps/infill_generator.cpp)
target_link_libraries(infill_generator PRIVATE cork_core)

add_executable(kitbash_merger apps/kitbash_merger.cpp)
target_link_libraries(kitbash_merger PRIVATE cork_core)

# Installation
install(TARGETS cork_core cork_cli surgical_planner infill_generator kitbash_merger
    EXPORT CorkTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
install(DIRECTORY src/ DESTINATION include/cork FILES_MATCHING PATTERN "*.h")
