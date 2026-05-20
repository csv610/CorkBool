#include <iostream>
using std::cin;
using std::cout;
using std::cerr;
using std::endl;
#include <string>
using std::string;
#include <vector>
using std::vector;

int main(int argc, const char* argv[])
{
    string input;
    if(!(cin >> input) || input != "OFF") {
        cerr << "Error: Invalid OFF header or failed to read. Expected 'OFF'." << endl;
        return 1;
    }
    
    int numvertices;
    int numfaces;
    int numedges;
    
    if(!(cin >> numvertices >> numfaces >> numedges) || numedges > 0) {
        cerr << "Error: Invalid counts or unsupported edges (must be 0)." << endl;
        return 1;
    }
    
    vector<double> vertices(numvertices*3);
    vector< vector<int> > faces(numfaces);
    
    for(int i=0; i<numvertices; i++)
    {
        cout << "v";
        for(int v=0; v<3; v++)
        {
            double coord;
            if (!(cin >> coord)) {
                cerr << "Error: Failed to read vertex coordinate." << endl;
                return 1;
            }
            cout << ' ' << coord;
        }
        cout << endl;
    }
    
    for(int i=0; i<numfaces; i++)
    {
        int vcount;
        if(!(cin >> vcount) || vcount < 3) {
            cerr << "Error: Invalid face vertex count (must be >= 3)." << endl;
            return 1;
        }
        
        cout << "f ";
        for(int v=0; v<vcount; v++)
        {
            int index;
            if (!(cin >> index)) {
                cerr << "Error: Failed to read face index." << endl;
                return 1;
            }
            cout << ' ' << index+1;
        }
        cout << endl;
    }
    
    return 0;
}
