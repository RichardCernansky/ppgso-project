#ifndef TREE_H
#define TREE_H

#include <ppgso/ppgso.h>
#include "../scene.cpp"

class Tree final : public Renderable {
    static std::unique_ptr<ppgso::Mesh> mesh;
    static std::unique_ptr<ppgso::Texture> texture;

public:
    glm::vec3 position{0, 0, 0};
    glm::vec3 scale{0.1f, 0.1f, 0.1f};
    glm::mat4 modelMatrix{1.0f};
    glm::mat4 childModelMatrix{1.0f};

    Tree();

    bool update(float dTime, Scene &scene);
    bool update_child(float dTime, Scene &scene, glm::mat4 ParentModelMatrix) override;
    void render(Scene &scene) override;
};
#endif
