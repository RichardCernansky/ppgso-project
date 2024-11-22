#ifndef CRYSTAL_H
#define CRYSTAL_H

#include <ppgso/ppgso.h>
#include "../scene.cpp"
#include "../renderable.h"

class Crystal final : public Renderable {
    glm::mat4 modelMatrix{1.0f};

    // Static resources
    static std::unique_ptr<ppgso::Mesh> mesh;
    static std::unique_ptr<ppgso::Texture> texture;

public:
    glm::vec3 position{0, 0.8, 0.15};  // Position of the crystal
    glm::vec3 scale{.08, .08, .08};     // Scale of the crystal
    glm::vec3 rotation{0, 0, 0};  // Rotation in radians

    // Constructor
    Crystal();

    // Update function
    bool update(float dTime, Scene &scene) override;
    void render(Scene &scene) override;
    bool update_child(float dTime, Scene &scene, glm::mat4 parentModelMatrix) override;

};

#endif // CRYSTAL_H
