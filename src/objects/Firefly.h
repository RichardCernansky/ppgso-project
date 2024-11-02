#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "src/InstancedMesh.h"
#include "src/scene.cpp"

class Firefly : public Renderable{
public:
    // Constructor: takes the count of fireflies and a mesh for the firefly model
    Firefly(int count);

    // Update method to animate fireflies
    bool update(float dTime,  Scene &scene) override;

    // Render method for instanced drawing
    void render(Scene &scene) override;

private:
    int fireflyCount;
    InstancedMesh fireflyMesh = {"sphere_firefly.obj"};

    std::vector<glm::vec3> initialPositions;
    std::vector<glm::vec3> positions;
    std::vector<glm::mat4> modelMatrices;

    GLuint modelMatVBO;
};
