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

        auto position = glm::vec3(
            obj["position"].value("x", 0.0f),
            obj["position"].value("y", 0.0f),
            obj["position"].value("z", 0.0f)
        );

        auto rotation = glm::vec3(
            obj["rotation"].value("x", 0.0f),
            obj["rotation"].value("y", 0.0f),
            obj["rotation"].value("z", 0.0f)
        );

        std::vector<glm::vec3> trajectoryPoints;
        if (obj.contains("trajectoryPoints")) {
            for (const auto& point : obj["trajectoryPoints"]) {
                trajectoryPoints.emplace_back(
                    point.value("x", 0.0f),
                    point.value("y", 0.0f),
                    point.value("z", 0.0f)
                );
            }
        }

        auto name = obj.value("name", "");
        auto objPath = rootDir + obj.value("objPath", "");
        auto mtlPath = rootDir + obj.value("mtlPath", "");
        auto texturePath = rootDir + obj.value("texturePath", "");
        auto scale = obj.value("scale", 0.0f);
        auto trajectorySpeed = obj.value("trajectorySpeed", 0.0f);
        o.initialize(name, objPath, mtlPath, texturePath, scale, position, rotation, trajectoryPoints, trajectorySpeed, shader);
        objects.push_back(o);
    }
    return objects;
}