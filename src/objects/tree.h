#ifndef TREE_H
#define TREE_H

#include <ppgso/ppgso.h>
#include "../scene.cpp"
#include "../renderable.h"

// Declare the Tree class
class Tree final : public Renderable {
    glm::mat4 modelMatrix{1.0f};

    // Static resources shared by all trees
    static std::unique_ptr<ppgso::Mesh> mesh;
    static std::unique_ptr<ppgso::Texture> texture;


public:
    glm::vec3 position{0, 0, -4};  // Position of the tree
    glm::vec3 scale{0.1f, 0.1f, 0.1f};  // Scale of the tree
    std::vector<glm::mat4> instanceTransforms;

    Tree();

    bool update(float dTime, Scene &scene) override;
    void render(Scene &scene) override;
    void render_instanced(Scene &scene, const std::vector<glm::mat4> &instanceTransforms);
    bool update_child(float d, Scene &scene, glm::mat4 ParentModelMatrix) override;
    void setLightShader(Scene &scene) const;
};

#endif // TREE_H
