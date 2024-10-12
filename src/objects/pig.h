//
// Created by Richard Čerňanský on 12/10/2024.
//
// pig.h

#include <ppgso/ppgso.h>
#include "../scene.cpp"
#include "src/renderable.h"

class Pig final : public Renderable {
private:
    glm::mat4 modelMatrix{1.0f};  // Model transformation matrix

    // Static resources (shared among all pigs)
    static std::unique_ptr<ppgso::Mesh> mesh;
    static std::unique_ptr<ppgso::Texture> texture;

public:
    // Pig position and scale
    glm::vec3 position{6.7, 0, -1.5};  // Initially placed at (0, 0, 0)
    glm::vec3 scale{1, 1, 1};     // Default scale

    Pig();  // Constructor

    // Update and render methods
    bool update(float dTime, Scene &scene) override;
    void render(Scene &scene) override;
};

