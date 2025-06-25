#pragma once

#include <string>
#include <vector>
#include "Object3D.h"
#include "Shader.h"

std::vector<Object3D> loadObjectConfigs(const std::string &configPath, const std::string &rootDir, Shader *shader);