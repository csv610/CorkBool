/**
 * Proof of Concept: Generative "Kitbashing" (Organic Splicing)
 * 
 * Demonstrates seamless boolean unions for procedural game art.
 * Combines base meshes with procedural appendages.
 */
#include <iostream>
#include "cork.h"
#include "../src/file_formats/files.h"

using namespace cork;

void load(const std::string& path, CorkTriMesh& out) {
    Files::FileMesh fm;
    if (Files::readTriMesh(path, &fm) != 0) throw CorkException(Error::FileLoadFailure, "Failed");
    for (const auto& v : fm.vertices) { out.vertices.push_back(v.pos.x); out.vertices.push_back(v.pos.y); out.vertices.push_back(v.pos.z); }
    for (const auto& t : fm.triangles) { out.triangles.push_back(t.a); out.triangles.push_back(t.b); out.triangles.push_back(t.c); }
}
void save(const std::string& path, const CorkTriMesh& in) {
    Files::FileMesh fm; fm.vertices.resize(in.vertices.size() / 3); fm.triangles.resize(in.triangles.size() / 3);
    for (size_t i = 0; i < fm.vertices.size(); ++i) { fm.vertices[i].pos.x = in.vertices[i*3]; fm.vertices[i].pos.y = in.vertices[i*3+1]; fm.vertices[i].pos.z = in.vertices[i*3+2]; }
    for (size_t i = 0; i < fm.triangles.size(); ++i) { fm.triangles[i].a = in.triangles[i*3]; fm.triangles[i].b = in.triangles[i*3+1]; fm.triangles[i].c = in.triangles[i*3+2]; }
    Files::writeTriMesh(path, &fm);
}

int main(int argc, char** argv) {
    if (argc != 4) {
        std::cerr << "Usage: kitbash_merger <base_creature.off> <appendage.off> <spliced_result.off>\n";
        return 1;
    }
    try {
        std::cout << "Loading base geometry and procedural appendage...\n";
        CorkTriMesh base, app;
        load(argv[1], base);
        load(argv[2], app);

        std::cout << "Performing seamless generative splice (Union)...\n";
        CorkTriMesh result = computeUnion(base, app);

        std::cout << "Saving organic joined mesh to " << argv[3] << "...\n";
        save(argv[3], result);
        std::cout << "Success. Mesh is watertight: " << (isSolid(result) ? "YES" : "NO") << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Operation failed: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
