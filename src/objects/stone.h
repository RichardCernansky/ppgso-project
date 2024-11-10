//
// Created by Bruno Kristián on 27/10/2024.
//
#include <ppgso/ppgso.h>
#include "../scene.cpp"
#include "src/renderable.h"

#ifndef STONE_H
#define STONE_H
class Stone final : public Renderable {
private:
    glm::mat4 modelMatrix{1.0f};
    static std::unique_ptr<ppgso::Mesh> mesh;
    static std::unique_ptr<ppgso::Texture> texture;

public:
    glm::vec3 position{10, 0, -6};
    glm::vec3 scale{0.04, 0.04, 0.04};
    float radius = 20*scale.x;


    Stone();
    bool update(float dTime, Scene &scene) override;
    void render(Scene &scene) override;
};
#endif
