#pragma once

#include <string>
#include "ObjectConfig.h"

using std::string;
void loadOBJ(Object3D *object);
void loadMTL(Object3D *object);
GLuint loadTexture(string filePath);