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
    glm::vec3 position{-3, 0.8, -2.85};  // Position of the crystal
    glm::vec3 scale{.07, .07, .07};     // Scale of the crystal
    glm::vec3 rotation{0, 0, 0};  // Rotation in radians

    // Constructor
    Crystal();

    // Update function
    bool update(float dTime, Scene &scene) override;

    // Render function
    void render(Scene &scene) override;

    // Optional: set light properties for the shader
    void setLightShader(Scene &scene) const;
};

#endif // CRYSTAL_H
