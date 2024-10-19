#ifndef TREE_H
#define TREE_H

#include <ppgso/ppgso.h>
#include "../scene.cpp"

// Declare the Tree class
class Tree final : public Renderable {
    // Static resources shared by all trees
    static std::unique_ptr<ppgso::Mesh> mesh;
    static std::unique_ptr<ppgso::Texture> texture;

public:
    std::vector<std::unique_ptr<Renderable>> children;  // scene hierarchy children objects
    glm::vec3 position{0, 0, 0};  // Position of the tree
    glm::vec3 scale{0.1f, 0.1f, 0.1f};  // Scale of the tree
    glm::mat4 modelMatrix{1.0f};

    Tree();

    bool update(float dTime, Scene &scene) override;
    void render(Scene &scene) override;
};

#endif // TREE_H
