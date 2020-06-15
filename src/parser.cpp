
#include <string>
#include <fstream>
#include <sstream>

#include "parser.h"

using std::string;

std::vector<shared_ptr<Primitive>> loadObjFile2(char const *filename) {
    std::vector<shared_ptr<Primitive>> trianglemesh;

    // Data we need to parse

    // Number of triangles
    int nTriangles;

    // Vector containing all the faces vertex indices
    // Face0 -> vertexIndices[0],  vertexIndices[1], vertexIndices[2]
    std::vector<int> vertexIndices;

    // All the vertices positions
    std::vector<Point> vertexPos;
    // All the vertices normals
    std::vector<Normal> vertexNorm;


    // Open file
    std::ifstream filestream(filename);

    // Parsing variables
    string line;
    string key;
    int vertexIndex = 0;
    Float x, y, z;
    shared_ptr<Material> lambert = make_shared<LambertianMaterial>(Color(0.5, 0.2, 0.5));
    
    if (filestream.is_open()) {
        while (std::getline(filestream, line)) {
            std::istringstream linestream(line);
            linestream >> key;
            if (key == "v") { // Vertex position
                linestream >> x >> y >> z;
                //vertexIndices.push_back(++vertexIndex);
                Point pt (x,y,z);
                vertexPos.emplace_back(pt);
                trianglemesh.emplace_back( make_shared<Sphere>(pt, 0.5, lambert) );
            }
            else if (key == "vn") { // Vertex normal
                linestream >> x >> y >> z;
                vertexNorm.emplace_back(Normal(x,y,z));
            }
            else if (key == "f") { // Faces
                ++nTriangles;
                // Store each vertex block into a string
                // v1p/v1n/v1t v2p/v2n/v2t etc..
                string vertexBlock;
                for (int i =0; i < 3; i++) {
                    linestream >> vertexBlock;
                    // Find pos of first slash
                    auto pos = vertexBlock.find("/");
                    // Vertex pos index is the first occcurence
                    int vertex = std::stoi(vertexBlock.substr(0, pos));
                    vertexIndices.emplace_back(vertex - 1); // Index needs to start from 0
                    // TODO: Store normals & uvs if present
                }


            }
        }
        // trianglemesh = CreateTriangleMesh( nTriangles, std::move(vertexIndices), 
                                                    //    std::move(vertexPos) );
    }
    return std::move(trianglemesh);
}

std::vector<shared_ptr<Primitive>> loadObjFile(char const *filename) {
    std::vector<shared_ptr<Primitive>> trianglemesh;

    // Data we need to parse

    // Number of triangles
    int nTriangles;

    // Vector containing all the faces vertex indices
    // Face0 -> vertexIndices[0],  vertexIndices[1], vertexIndices[2]
    std::vector<int> vertexIndices;

    // All the vertices positions
    std::vector<Point> vertexPos;
    // All the vertices normals
    std::vector<Normal> vertexNorm;


    // Open file
    std::ifstream filestream(filename);

    // Parsing variables
    string line;
    string key;
    int vertexIndex = 0;
    Float x, y, z;
    
    if (filestream.is_open()) {
        while (std::getline(filestream, line)) {
            std::istringstream linestream(line);
            linestream >> key;
            if (key == "v") { // Vertex position
                linestream >> x >> y >> z;
                //vertexIndices.push_back(++vertexIndex);
                vertexPos.emplace_back(Point(x,y,z));
            }
            else if (key == "vn") { // Vertex normal
                linestream >> x >> y >> z;
                vertexNorm.emplace_back(Normal(x,y,z));
            }
            else if (key == "f") { // Faces
                ++nTriangles;
                // Store each vertex block into a string
                // v1p/v1n/v1t v2p/v2n/v2t etc..
                string vertexBlock;
                for (int i =0; i < 3; i++) {
                    linestream >> vertexBlock;
                    // Find pos of first slash
                    auto pos = vertexBlock.find("/");
                    // Vertex pos index is the first occcurence
                    int vertex = std::stoi(vertexBlock.substr(0, pos));
                    vertexIndices.emplace_back(vertex - 1); // Index needs to start from 0
                    // TODO: Store normals & uvs if present
                }


            }
        }
        trianglemesh = CreateTriangleMesh( nTriangles, std::move(vertexIndices), 
                                                       std::move(vertexPos) );
    }
    return std::move(trianglemesh);
}