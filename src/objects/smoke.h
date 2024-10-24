//
// Created by Bruno Kristián on 21/10/2024.
//

#ifndef SMOKE_H
#define SMOKE_H

#include <ppgso/ppgso.h>
#include "../scene.cpp"
#include "../renderable.h"

class Smoke final : public Renderable {
    glm::mat4 modelMatrix{1.0f};
    std::vector<std::unique_ptr<Renderable>> children;
    glm::mat4 parentPos{0};

    static std::unique_ptr<ppgso::Mesh> mesh;
    static std::unique_ptr<ppgso::Texture> texture;

public:
    glm::vec3 scale{100, 100, 100}; // Adjust as needed
    glm::vec3 position{0, 500,300};

    Smoke();

    void render(Scene &scene) override;
    bool update(float dTime, Scene &scene) override;
    bool update_child(float dTime, Scene &scene, glm::mat4 ParetModelMatrix) override;

};
#endif