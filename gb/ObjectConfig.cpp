#include <fstream>
#include <iostream>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "ObjectConfig.h"

std::vector<Object3D> loadObjectConfigs(const std::string &configPath, const std::string &rootDir)
{
    std::cout << "Loading object configurations from: " << configPath << std::endl;
    std::vector<Object3D> objects;
    std::ifstream file(configPath);
    std::cout << "Opening config file: " << configPath << std::endl;
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
        o.name = obj.value("name", "");
        o.objPath = rootDir + obj.value("objPath", "");
        o.mtlPath = rootDir + obj.value("mtlPath", "");
        o.texturePath = rootDir + obj.value("texturePath", "");
        o.scale = obj.value("scale", 0.0f);
        o.position = {
            obj["position"].value("x", 0.0f),
            obj["position"].value("y", 0.0f),
            obj["position"].value("z", 0.0f),
        };
        o.rotation = {
            obj["rotation"].value("x", 0.0f),
            obj["rotation"].value("y", 0.0f),
            obj["rotation"].value("z", 0.0f),
        };
        o.isSelected = i == 0;
        objects.push_back(o);
    }
    return objects;
}