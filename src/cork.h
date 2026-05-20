// +-------------------------------------------------------------------------
// | cork.h
// | 
// | Author: Gilbert Bernstein
// +-------------------------------------------------------------------------
#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include <system_error>

namespace cork {

using uint = std::uint32_t;

/**
 * @brief A simple triangle mesh structure.
 * 
 * Uses RAII via std::vector for automatic memory management.
 */
struct CorkTriMesh
{
    std::vector<uint> triangles; // 3 indices per triangle
    std::vector<float> vertices; // 3 coordinates per vertex
};

/**
 * @brief Error codes for Cork operations.
 */
enum class Error {
    Success = 0,
    EmptyMesh,
    InvalidVertexReference,
    SelfIntersecting,
    NotClosed,
    FileLoadFailure,
    FileSaveFailure
};

/**
 * @brief Custom exception for Cork-specific errors.
 */
class CorkException : public std::runtime_error {
public:
    explicit CorkException(Error err, const std::string& msg) 
        : std::runtime_error(msg), error_code(err) {}
    Error code() const { return error_code; }
private:
    Error error_code;
};

/**
 * @brief Test whether a mesh is solid (closed and non-self-intersecting).
 * 
 * @param mesh The mesh to test.
 * @return true If solid.
 * @return false If not solid.
 */
bool isSolid(const CorkTriMesh& mesh);

/**
 * @brief Computes Boolean operations on triangle meshes.
 * 
 * Inputs must be solid and counter-clockwise oriented.
 */
CorkTriMesh computeUnion(const CorkTriMesh& a, const CorkTriMesh& b);
CorkTriMesh computeDifference(const CorkTriMesh& a, const CorkTriMesh& b);
CorkTriMesh computeIntersection(const CorkTriMesh& a, const CorkTriMesh& b);
CorkTriMesh computeSymmetricDifference(const CorkTriMesh& a, const CorkTriMesh& b);

/**
 * @brief Resolve intersections between two surfaces.
 */
CorkTriMesh resolveIntersections(const CorkTriMesh& a, const CorkTriMesh& b);

} // namespace cork
