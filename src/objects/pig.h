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
    // Initially placed at (0, 0, 0)
    glm::vec3 scale{1, 1, 1};     // Default scale
    glm::vec3 globalDirection{0,0,1};
    glm::vec3 initialPosition{0, 0, 0};

    float timeInState = 0.0f;
    float changeDirectionTime = 0.0f;
    float wolfCollisionThreshold = 2.0f;

    float runDuration = 0.0f;       // The current time elapsed during the run-off phase
    float maxRunDuration = 4.0f;    // Random duration between 3 to 5 seconds for each run-off
    float currentSpeed = 0.0f;      // Current speed of the pig during run-off
    glm::vec3 runDirection;         // The direction in which the pig is running off
    bool isRunningOff = false;      // Flag to indicate if the pig is in the run-off phase

    Pig();  // Constructor

    // Update and render methods
    bool update(float dTime, Scene &scene) override;
    void render(Scene &scene) override;
    bool isCollided(const Scene &scene) const;
    void run_off();
};

