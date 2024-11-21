//
// Created by Bruno Kristián on 18/10/2024.
//

#include <ppgso/ppgso.h>
#include "../scene.cpp"
#include "src/renderable.h"

#ifndef APPLE_TREE_H
#define APPLE_TREE_H

class AppleTree final : public Renderable {
private:
    glm::mat4 modelMatrix{1.0f};  // Model transformation matrix

    // Static resources (shared among all pigs)
    static std::unique_ptr<ppgso::Mesh> mesh;
    static std::unique_ptr<ppgso::Texture> texture;

public:
    std::vector<std::unique_ptr<Renderable>> children;
    glm::vec3 position{5, 0, 7.5};
    glm::vec3 scale{1, 1, 1};

    AppleTree();  // Constructor

    // Update and render methods
    bool update(float dTime, Scene &scene) override;

    void render(Scene &scene) override;
};

#endif //APPLE_TREE_H
