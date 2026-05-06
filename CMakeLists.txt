cmake_minimum_required(VERSION 3.16)
project(cork VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GMP REQUIRED gmp gmpxx)

add_library(cork INTERFACE)
target_include_directories(cork INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/util
    ${CMAKE_CURRENT_SOURCE_DIR}/src/math
    ${CMAKE_CURRENT_SOURCE_DIR}/src/isct
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mesh
    ${CMAKE_CURRENT_SOURCE_DIR}/src/rawmesh
    ${CMAKE_CURRENT_SOURCE_DIR}/src/accel
    ${CMAKE_CURRENT_SOURCE_DIR}/src/file_formats
    ${GMP_INCLUDE_DIRS}
)
target_link_libraries(cork INTERFACE ${GMP_LIBRARIES})
target_compile_options(cork INTERFACE
    $<$<CXX_COMPILER_ID:Clang>:-stdlib=libc++>
)

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
target_include_directories(cork_core PRIVATE ${GMP_INCLUDE_DIRS})
target_link_libraries(cork_core PUBLIC cork)
set_source_files_properties(src/isct/triangle.c PROPERTIES
    COMPILE_OPTIONS "-O2;-DNO_TIMER;-DREDUCED;-DCDT_ONLY;-DTRILIBRARY;-Wall;-DANSI_DECLARATORS"
)

add_executable(cork_cli src/main.cpp)
target_link_libraries(cork_cli PRIVATE cork_core)

add_executable(off2obj src/off2obj.cpp)
target_link_libraries(off2obj PRIVATE cork_core)

enable_testing()

add_executable(cork_test tests/cork_test.cpp)
target_link_libraries(cork_test PRIVATE cork_core)

add_test(NAME cork_test COMMAND cork_test)

install(TARGETS cork_core off2obj
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)