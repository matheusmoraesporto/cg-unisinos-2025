#include <fstream>
#include <iostream>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "Shader.h"
#include "Camera.h"
#include "ObjectConfig.h"

void loadObjectConfigs(
    const std::string &configPath,
    const std::string &rootDir,
    std::vector<Object3D> *objects,
    Camera *camera,
    Shader *shader,
    int width,
    int height)
{
    std::ifstream file(configPath);
    if (!file.is_open())
    {
        std::cerr << "Failed to open config file: " << configPath << std::endl;
        return;
    }

    json j;
    file >> j;

    camInit(camera, shader, width, height, j);
    objsInit(objects, rootDir, j, shader);
}

void camInit(Camera *camera, Shader *shader, int width, int height, json j)
{
    auto jCam = j["camera"];

    auto front = glm::vec3(
        jCam["front"].value("x", 0.0f),
        jCam["front"].value("y", 0.0f),
        jCam["front"].value("z", 0.0f));

    auto position = glm::vec3(
        jCam["position"].value("x", 0.0f),
        jCam["position"].value("y", 0.0f),
        jCam["position"].value("z", 0.0f));

    auto up = glm::vec3(
        jCam["up"].value("x", 0.0f),
        jCam["up"].value("y", 0.0f),
        jCam["up"].value("z", 0.0f));

    camera->initialize(
        shader,
        width,
        height,
        jCam.value("sensitivity", 0.0f),
        jCam.value("pitch", 0.0f),
        jCam.value("yaw", 0.0f),
        jCam.value("speed", 0.0f),
        front,
        position,
        up);
}

void objsInit(
    std::vector<Object3D> *objects,
    const std::string &rootDir,
    const json &j,
    Shader *shader)
{
    for (size_t i = 0; i < j["objects"].size(); ++i)
    {
        const auto &obj = j["objects"][i];
        Object3D o;

        auto position = glm::vec3(
            obj["position"].value("x", 0.0f),
            obj["position"].value("y", 0.0f),
            obj["position"].value("z", 0.0f));

        auto rotation = glm::vec3(
            obj["rotation"].value("x", 0.0f),
            obj["rotation"].value("y", 0.0f),
            obj["rotation"].value("z", 0.0f));

        auto lightingBack = glm::vec3(
            obj["lighting"]["back"].value("x", 0.0f),
            obj["lighting"]["back"].value("y", 0.0f),
            obj["lighting"]["back"].value("z", 0.0f));

        auto lightingKey = glm::vec3(
            obj["lighting"]["key"].value("x", 0.0f),
            obj["lighting"]["key"].value("y", 0.0f),
            obj["lighting"]["key"].value("z", 0.0f));

        auto lightingFill = glm::vec3(
            obj["lighting"]["fill"].value("x", 0.0f),
            obj["lighting"]["fill"].value("y", 0.0f),
            obj["lighting"]["fill"].value("z", 0.0f));

        o.initialize(
            obj.value("name", ""),
            rootDir + obj.value("objPath", ""),
            rootDir + obj.value("mtlPath", ""),
            rootDir + obj.value("texturePath", ""),
            obj.value("scale", 0.0f),
            position,
            rotation,
            getTrajectoryPoints(obj),
            obj.value("trajectorySpeed", 0.0f),
            lightingBack,
            lightingKey,
            lightingFill,
            shader);

        objects->push_back(o);
    }
}

std::vector<glm::vec3> getTrajectoryPoints(const json &obj)
{
    std::vector<glm::vec3> trajectoryPoints;
    if (obj.contains("trajectoryPoints"))
    {
        for (const auto &point : obj["trajectoryPoints"])
        {
            trajectoryPoints.emplace_back(
                point.value("x", 0.0f),
                point.value("y", 0.0f),
                point.value("z", 0.0f));
        }
    }

    return trajectoryPoints;
}