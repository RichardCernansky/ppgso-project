//
// Created by Bruno Kristián on 21/10/2024.
//

#ifndef SMOKE_H
#define SMOKE_H

#include <ppgso/ppgso.h>
#include "../scene.cpp"
#include "../renderable.h"

class Smoke final : public Renderable {
    // Transformation matrices
    glm::mat4 modelMatrix{1.0f};

    // Static resources shared by all apples
    static std::unique_ptr<ppgso::Mesh> mesh;
    static std::unique_ptr<ppgso::Texture> texture;

public:
    glm::vec3 scale{0.1, 0.1, 0.1}; // Adjust as needed
    glm::vec3 position{0, 0.5,-0.8};

    Smoke();

    void render(Scene &scene) override;
    bool update(float dTime, Scene &scene) override;
    bool isTransparent() const override { return true; }
    glm::vec3 getPosition() const override { return position; }

};
#endif