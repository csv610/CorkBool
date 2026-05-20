#include "files.h"
#include "cork.h"
#include <iostream>
#include <chrono>
#include <string>
#include <iomanip>

using namespace cork;

static void file2corktrimesh(const Files::FileMesh &in, CorkTriMesh &out) {
    out.vertices.clear();
    out.triangles.clear();
    out.vertices.reserve(in.vertices.size() * 3);
    out.triangles.reserve(in.triangles.size() * 3);
    
    for(const auto& v : in.vertices) {
        out.vertices.push_back(static_cast<float>(v.pos.x));
        out.vertices.push_back(static_cast<float>(v.pos.y));
        out.vertices.push_back(static_cast<float>(v.pos.z));
    }
    for(const auto& t : in.triangles) {
        out.triangles.push_back(t.a);
        out.triangles.push_back(t.b);
        out.triangles.push_back(t.c);
    }
}

static CorkTriMesh loadMesh(const std::string& filename) {
    Files::FileMesh filemesh;
    if(Files::readTriMesh(filename, &filemesh) > 0) {
        throw CorkException(Error::FileLoadFailure, "Unable to load " + filename);
    }
    CorkTriMesh out;
    file2corktrimesh(filemesh, out);
    return out;
}

template<typename Func>
void run_benchmark(const std::string& name, Func func) {
    std::cout << std::left << std::setw(25) << name << ": ";
    
    auto start = std::chrono::high_resolution_clock::now();
    
    try {
        CorkTriMesh result = func();
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> ms = end - start;
        std::cout << std::right << std::setw(10) << std::fixed << std::setprecision(2) << ms.count() << " ms "
                  << "(Output: " << result.triangles.size() / 3 << " tris)" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "FAILED (" << e.what() << ")" << std::endl;
    }
}

int main(int argc, char** argv) {
    std::string sample_dir = "../samples/";
    if (argc > 1) {
        sample_dir = argv[1];
        if (sample_dir.back() != '/' && sample_dir.back() != '\\') {
            sample_dir += "/";
        }
    }

    std::cout << "Loading sample meshes from: " << sample_dir << std::endl;
    CorkTriMesh meshA, meshB;
    try {
        meshA = loadMesh(sample_dir + "ballA.off");
        meshB = loadMesh(sample_dir + "ballB.off");
        std::cout << "Mesh A: " << meshA.triangles.size() / 3 << " triangles, " 
                  << meshA.vertices.size() / 3 << " vertices\n";
        std::cout << "Mesh B: " << meshB.triangles.size() / 3 << " triangles, "
                  << meshB.vertices.size() / 3 << " vertices\n";
    } catch (const std::exception& e) {
        std::cerr << "Failed to load samples. Please run from the 'build' directory or provide path.\nError: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "\nRunning Benchmarks..." << std::endl;
    std::cout << std::string(60, '-') << std::endl;

    run_benchmark("computeUnion", [&]() { return computeUnion(meshA, meshB); });
    run_benchmark("computeIntersection", [&]() { return computeIntersection(meshA, meshB); });
    run_benchmark("computeDifference", [&]() { return computeDifference(meshA, meshB); });
    run_benchmark("computeSymmetricDiff", [&]() { return computeSymmetricDifference(meshA, meshB); });
    run_benchmark("resolveIntersections", [&]() { return resolveIntersections(meshA, meshB); });

    std::cout << std::string(60, '-') << std::endl;
    return 0;
}
