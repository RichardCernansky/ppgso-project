//
// Created by Bruno Kristián on 21/10/2024.
//

#ifndef FIRE_H
#define FIRE_H

#include <ppgso/ppgso.h>
#include "../scene.cpp"
#include "../renderable.h"

class Fire final : public Renderable {
    glm::mat4 modelMatrix{1.0f};

    static std::unique_ptr<ppgso::Mesh> mesh;
    static std::unique_ptr<ppgso::Texture> texture;

public:
    glm::vec3 scale{0.001f, 0.001f, 0.001f};
    glm::vec3 position{0, 0,-1};

    Fire();

    void render(Scene &scene) override;
    bool update_child(float dTime, Scene &scene, glm::mat4 ParentModelMatrix) override;
    bool update(float dTime, Scene &scene) override;

};
#endif
