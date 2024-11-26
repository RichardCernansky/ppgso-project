#include <ppgso/ppgso.h>
#include "../scene.cpp"
#include "src/renderable.h"

#ifndef APPLE_TREE_H
#define APPLE_TREE_H

class AppleTree final : public Renderable {
private:
    glm::mat4 modelMatrix{1.0f};  // Model transformation matrix
    static std::unique_ptr<ppgso::Mesh> mesh;
    static std::unique_ptr<ppgso::Texture> texture;

public:
    glm::vec3 position{0, 0, -10};
    glm::vec3 scale{1, 1, 1};

    AppleTree();

    bool update(float dTime, Scene &scene) override;
    bool update_child(float dTime, Scene &scene, glm::mat4 ParentModelMatrix) override;
    void render(Scene &scene) override;
};

#endif
