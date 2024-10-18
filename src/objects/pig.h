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
    std::vector<std::unique_ptr<Renderable>> children;  // scene hierarchy children objects
    // Pig position and scale
    glm::vec3 position{5, 0, -10};  // Initially placed at (0, 0, 0)
    glm::vec3 scale{1, 1, 1};     // Default scale
    glm::vec3 globalDirection{0,0,1};
    float timeInState = 0.0f;

    Pig();  // Constructor

    // Update and render methods
    bool update(float dTime, Scene &scene) override;
    void render(Scene &scene) override;
};

