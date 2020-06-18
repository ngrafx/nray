#pragma once

// Parsing functions
// reads files and returns classes
// for the renderer

#include <string>

#include "nray.h"
#include "primitive.h"

using std::string;

enum class SceneItem
{
    Settings,
    Camera,
    Sphere,
    ObjMesh,
    Material,
    Environment,
    Unknown
};

SceneItem ToSceneItem(string const &str);
shared_ptr<Material> CreateMaterial(string const &line);

std::vector<shared_ptr<Primitive>> LoadObjFile(char const *filename, shared_ptr<Material> material);

Scene LoadSceneFile(char const *filename);