#pragma once

#include <string>
#include <vector>
#include "Object3D.h"
#include "Shader.h"
#include <nlohmann/json.hpp>
using json = nlohmann::json;

void loadObjectConfigs(
    const std::string &configPath,
    const std::string &rootDir,
    std::vector<Object3D> *objects,
    Camera *camera,
    Shader *shader,
    int width,
    int height);

void camInit(
    Camera *camera,
    Shader *shader,
    int width,
    int height,
    json j);

void objsInit(
    std::vector<Object3D> *objects,
    const std::string &rootDir,
    const json &j,
    Shader *shader);

std::vector<glm::vec3> getTrajectoryPoints(const json &obj);