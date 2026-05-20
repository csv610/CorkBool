/**
 * Proof of Concept: Surgical "Digital Twin" Planner
 * 
 * Demonstrates high-precision boolean subtraction.
 * In a real application, 'target' is a patient's bone scan,
 * and 'tool' is a virtual surgical saw or drill.
 */
#include <iostream>
#include "cork.h"
#include "../src/file_formats/files.h"

using namespace cork;

void load(const std::string& path, CorkTriMesh& out) {
    Files::FileMesh fm;
    if (Files::readTriMesh(path, &fm) != 0) throw CorkException(Error::FileLoadFailure, "Failed to load " + path);
    out.vertices.reserve(fm.vertices.size() * 3);
    out.triangles.reserve(fm.triangles.size() * 3);
    for (const auto& v : fm.vertices) {
        out.vertices.push_back(v.pos.x); out.vertices.push_back(v.pos.y); out.vertices.push_back(v.pos.z);
    }
    for (const auto& t : fm.triangles) {
        out.triangles.push_back(t.a); out.triangles.push_back(t.b); out.triangles.push_back(t.c);
    }
}

void save(const std::string& path, const CorkTriMesh& in) {
    Files::FileMesh fm;
    fm.vertices.resize(in.vertices.size() / 3);
    fm.triangles.resize(in.triangles.size() / 3);
    for (size_t i = 0; i < fm.vertices.size(); ++i) {
        fm.vertices[i].pos.x = in.vertices[i*3]; fm.vertices[i].pos.y = in.vertices[i*3+1]; fm.vertices[i].pos.z = in.vertices[i*3+2];
    }
    for (size_t i = 0; i < fm.triangles.size(); ++i) {
        fm.triangles[i].a = in.triangles[i*3]; fm.triangles[i].b = in.triangles[i*3+1]; fm.triangles[i].c = in.triangles[i*3+2];
    }
    Files::writeTriMesh(path, &fm);
}

int main(int argc, char** argv) {
    if (argc != 4) {
        std::cerr << "Usage: surgical_planner <bone_mesh.off> <surgical_tool.off> <cut_result.off>\n";
        return 1;
    }
    try {
        std::cout << "Loading patient data and surgical path...\n";
        CorkTriMesh bone, tool;
        load(argv[1], bone);
        load(argv[2], tool);

        std::cout << "Simulating exact surgical cut (Difference)...\n";
        CorkTriMesh result = computeDifference(bone, tool);

        std::cout << "Saving cut bone for Finite Element Analysis (FEA) at " << argv[3] << "...\n";
        save(argv[3], result);
        std::cout << "Success. Mesh is watertight: " << (isSolid(result) ? "YES" : "NO") << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Operation failed: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
