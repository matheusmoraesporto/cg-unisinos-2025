#include <fstream>
#include <iostream>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "Shader.h"
#include "ObjectConfig.h"

std::vector<Object3D> loadObjectConfigs(const std::string &configPath, const std::string &rootDir, Shader *shader)
{
    std::vector<Object3D> objects;
    std::ifstream file(configPath);
    if (!file.is_open())
    {
        std::cerr << "Failed to open config file: " << configPath << std::endl;
        return objects;
    }

    json j;
    file >> j;

    for (size_t i = 0; i < j["objects"].size(); ++i)
    {
        const auto &obj = j["objects"][i];
        Object3D o;

        Matrix position = {
            obj["position"].value("x", 0.0f),
            obj["position"].value("y", 0.0f),
            obj["position"].value("z", 0.0f),
        };

        Matrix rotation = {
            obj["rotation"].value("x", 0.0f),
            obj["rotation"].value("y", 0.0f),
            obj["rotation"].value("z", 0.0f),
        };

        auto name = obj.value("name", "");
        auto objPath = rootDir + obj.value("objPath", "");
        auto mtlPath = rootDir + obj.value("mtlPath", "");
        auto texturePath = rootDir + obj.value("texturePath", "");
        auto scale = obj.value("scale", 0.0f);
        o.initialize(name, objPath, mtlPath, texturePath, scale, position, rotation, shader);
        objects.push_back(o);
    }
    return objects;
}