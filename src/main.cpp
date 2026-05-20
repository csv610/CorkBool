#include "files.h"
#include "cork.h"
#include <iostream>
#include <vector>
#include <string>
#include <functional>

using namespace std;
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

static void corktrimesh2file(const CorkTriMesh &in, Files::FileMesh &out) {
    out.vertices.resize(in.vertices.size() / 3);
    out.triangles.resize(in.triangles.size() / 3);
    
    for(size_t i=0; i < out.vertices.size(); i++) {
        out.vertices[i].pos.x = in.vertices[3*i+0];
        out.vertices[i].pos.y = in.vertices[3*i+1];
        out.vertices[i].pos.z = in.vertices[3*i+2];
    }
    for(size_t i=0; i < out.triangles.size(); i++) {
        out.triangles[i].a = in.triangles[3*i+0];
        out.triangles[i].b = in.triangles[3*i+1];
        out.triangles[i].c = in.triangles[3*i+2];
    }
}

static CorkTriMesh loadMesh(const string& filename) {
    Files::FileMesh filemesh;
    if(Files::readTriMesh(filename, &filemesh) > 0) {
        throw CorkException(Error::FileLoadFailure, "Unable to load " + filename);
    }
    CorkTriMesh out;
    file2corktrimesh(filemesh, out);
    return out;
}

static void saveMesh(const string& filename, const CorkTriMesh& mesh) {
    Files::FileMesh filemesh;
    corktrimesh2file(mesh, filemesh);
    if(Files::writeTriMesh(filename, &filemesh) > 0) {
        throw CorkException(Error::FileSaveFailure, "Unable to save " + filename);
    }
}

class CmdList {
public:
    void regCmd(string name, string helptxt, 
                std::function<void(vector<string>::iterator &, const vector<string>::iterator &)> body) {
        commands.push_back({name, helptxt, body});
    }
    
    void printHelp(ostream &out) {
        out << "Welcome to Cork (Refactored). Usage:" << endl;
        out << "  > cork [-command arg0 arg1 ... argn]*" << endl;
        for(const auto &cmd : commands) out << cmd.helptxt << endl;
    }

    void runCommands(vector<string>::iterator &it, const vector<string>::iterator &end) {
        while(it != end) {
            string arg = *it;
            if(arg[0] != '-') throw runtime_error("Commands must start with '-': " + arg);
            string name = arg.substr(1);
            ++it;
            bool found = false;
            for(const auto &cmd : commands) {
                if(cmd.name == name) {
                    cmd.body(it, end);
                    found = true;
                    break;
                }
            }
            if(!found) throw runtime_error("Unknown command: -" + name);
        }
    }
private:
    struct Command { string name, helptxt; std::function<void(vector<string>::iterator &, const vector<string>::iterator &)> body; };
    vector<Command> commands;
};

int main(int argc, char *argv[]) {
    if(argc < 2) {
        cout << "Use 'cork -help' for instructions." << endl;
        return 0;
    }

    CmdList cmds;
    cmds.regCmd("help", "-help: Show help", [&](auto&, auto) { cmds.printHelp(cout); });
    
    cmds.regCmd("solid", "-solid <in>: Check if mesh is solid", [](auto &args, auto end) {
        if(args == end) throw runtime_error("Missing argument for -solid");
        string file = *args++;
        cout << file << " is " << (isSolid(loadMesh(file)) ? "SOLID" : "NOT SOLID") << endl;
    });

    auto binaryOp = [](auto func) {
        return [func](auto &args, auto end) {
            if(distance(args, end) < 3) throw runtime_error("Binary op needs 3 args (in0, in1, out)");
            string in0 = *args++, in1 = *args++, out = *args++;
            saveMesh(out, func(loadMesh(in0), loadMesh(in1)));
        };
    };

    cmds.regCmd("union", "-union <in0> <in1> <out>", binaryOp(computeUnion));
    cmds.regCmd("diff", "-diff <in0> <in1> <out>", binaryOp(computeDifference));
    cmds.regCmd("isct", "-isct <in0> <in1> <out>", binaryOp(computeIntersection));
    cmds.regCmd("xor", "-xor <in0> <in1> <out>", binaryOp(computeSymmetricDifference));
    cmds.regCmd("resolve", "-resolve <in0> <in1> <out>", binaryOp(resolveIntersections));

    try {
        vector<string> args(argv + 1, argv + argc);
        auto it = args.begin();
        cmds.runCommands(it, args.end());
    } catch (const exception &e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    return 0;
}
