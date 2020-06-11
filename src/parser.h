#pragma once

#include "nray.h"
#include "primitive.h"


std::vector<shared_ptr<Primitive>> loadObjFile(char const *filename);

shared_ptr<Primitive> loadSceneFile(char const *filename);