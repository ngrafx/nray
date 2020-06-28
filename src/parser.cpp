#include <fstream>
#include <sstream>

#include "parser.h"
#include "scene.h"


SceneItem ToSceneItem(string const &str) {
    if (str == "<Settings>")
        return SceneItem::Settings;
    else if (str == "<Camera>")
        return SceneItem::Camera;
    else if (str == "<Sphere>")
        return SceneItem::Sphere;
    else if (str == "<ObjMesh>")
        return SceneItem::ObjMesh;
    else if (str == "<Material>")
        return SceneItem::Material;
    else if (str == "<Environment>")
        return SceneItem::Environment;

    return SceneItem::Unknown;
}

shared_ptr<Material> CreateMaterial(string const &line) {
    shared_ptr<Material> material;
    std::istringstream linestream(line);
    string key, mtl;
    Float r, g, b, val;
    linestream >> key >> mtl;
    if (mtl == "Lambertian") {
        linestream >> r >> g >> b;
        material = make_shared<LambertianMaterial>(Color(r,g,b));
    }
    else if (mtl == "Dielectric") {
        linestream >> r >> g >> b >> val;
        material = make_shared<DielectricMaterial>(Color(r,g,b), val);
    }
    else if (mtl == "Metal") {
        linestream >> r >> g >> b >> val;
        material = make_shared<MetalMaterial>(Color(r,g,b), val);
    }
    else if (mtl == "Emissive") {
        linestream >> r >> g >> b;
        material = make_shared<EmissiveMaterial>(Color(r,g,b));
    }
    else {
        std::cout << "Could not identify material: " << mtl << "\n";
        throw "Unknown Material";
    }
    return material;
}


std::vector<shared_ptr<Primitive>> LoadObjFile(char const *filename, shared_ptr<Material> material) {

    std::cerr << "Loading obj file: " << filename << "\n";

    std::vector<shared_ptr<Primitive>> trianglemesh;

    // Data we need to parse

    // Number of triangles
    int nTriangles = 0;

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
    Float x, y, z;
    
    if (!filestream.is_open()) {
        std::cerr << "Error opening obj file\n" ;
        throw "Failed opening obj file";
    }

    while (std::getline(filestream, line)) {
        std::istringstream linestream(line);
        linestream >> key;
        if (key == "v") { // Vertex position
            linestream >> x >> y >> z;
            vertexPos.push_back(Point(x,y,z));
        }
        else if (key == "vn") { // Vertex normal
            linestream >> x >> y >> z;
            vertexNorm.push_back(Normal(x,y,z));
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
                int vertex;
                if (pos != string::npos)
                    vertex = std::stoi(vertexBlock.substr(0, pos));
                else
                    vertex = std::stoi(vertexBlock);
                vertexIndices.push_back(vertex - 1); // Index needs to start from 0
                // TODO: Store uvs if present
            }
        }
        key = "";
    }

    // std::cout << "Vertex pos: " << vertexPos.size() << " Vertex norm: " << vertexNorm.size() << "\n";
    // std::cout << "Faces: " << vertexIndices.size() << " -> " << vertexIndices.size() / 3 << "\n";

    trianglemesh = CreateTriangleMesh( nTriangles, std::move(vertexIndices), 
                                        std::move(vertexPos), std::move(vertexNorm), material);

    return std::move(trianglemesh);
    // return trianglemesh;
}

Scene LoadSceneFile(char const *filename) {

    // Open file
    std::ifstream filestream(filename);
    if (!filestream.is_open())
        throw std::runtime_error("Failed opening scene file");

    // Scene
    RenderSettings options;
    // shared_ptr<PrimitiveList> world = make_shared<PrimitiveList>();
    PrimitiveList world;

    // Camera attributes
    Vec3 lookfrom, lookat, vup;
    Float vfov, aperture, focus_dist;
    int dof;

    // Default (and current) Material
    shared_ptr<Material> material = make_shared<LambertianMaterial> (Color(1,0,1));

    // Parse scene
    Image ibl;
    bool has_settings = false;
    bool has_camera = false;
    std::vector<string> objs_to_load;
    std::vector<shared_ptr<Material>> objs_materials;
    string line;
    string key;
    string path;

    while (std::getline(filestream, line)) {
        std::istringstream linestream(line);
        linestream >> key;

        Float x, y, z, val;
        

        switch(ToSceneItem(key)) {
            case SceneItem::Settings :
                has_settings = true;
                linestream >> options.image_width;
                linestream >> options.image_height;
                linestream >> options.pixel_samples;
                linestream >> options.max_ray_depth;
                break;

            case SceneItem::Camera :
                has_camera = true;
                // Lookfrom
                linestream >> x >> y >> z;
                lookfrom = Vec3(x, y, z);
                // Lookat
                linestream >> x >> y >> z;
                lookat = Vec3(x, y, z);
                // Vup
                linestream >> x >> y >> z;
                vup = Vec3(x, y, z);  
                linestream >> vfov;
                linestream >> aperture;
                linestream >> focus_dist;
                linestream >> dof;
                break;            

            case SceneItem::Sphere :
                linestream >> x >> y >> z >> val;
                world.add(make_shared<Sphere>(Point(x, y, z), val, material));
                break;

            case SceneItem::ObjMesh :
                linestream >> path;
                // Add the obj file path & the current material
                objs_to_load.push_back(path);
                objs_materials.push_back(material);
                path = "";
                break;

            case SceneItem::Material :
                material = CreateMaterial(line);
                break;

            case SceneItem::Environment :
                linestream >> x >> y >> z;
                linestream >> path;
                ibl.LoadFromFile(path.c_str());
                path = "";
                break;

            case SceneItem::Unknown :
                // std::cerr << "Warning: Unknown descriptor " << key << "\n";
                break;
            
        }
        key = "";
    }

    if (!has_settings || !has_camera) {
        throw std::runtime_error("Scene is missing <Settings> or <Camera>");
    }

    if (!objs_to_load.empty()) {
        for (int i=0; i<objs_to_load.size(); i++) {
            std::vector<shared_ptr<Primitive>> trianglemesh = LoadObjFile(objs_to_load[i].c_str(), objs_materials[i]);
            world.add(std::move(trianglemesh));
        }
    }

    // Init camera
    options.image_aspect_ratio = Float(options.image_width) / options.image_height;
    Camera cam(lookfrom, lookat, vup, vfov, options.image_aspect_ratio, aperture, focus_dist, dof==1);

    // Create BVH
    std::cout << "Creating BVH...\n";
    shared_ptr<BVH> bvh = make_shared<BVH>(world, 0.0, 0.0);

    Scene scene(bvh, cam, options, std::move(ibl));
    return std::move(scene);
}