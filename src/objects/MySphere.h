#include <ppgso/ppgso.h>
#include "../scene.cpp"
#include "../renderable.h"

class MySphere final : public Renderable {
    glm::mat4 modelMatrix{1.0f};

    // Static resources
    static std::unique_ptr<ppgso::Mesh> mesh;
    static std::unique_ptr<ppgso::Texture> texture;

public:
    // Position, rotation, and scale of the sphere
    glm::vec3 position{-3, 1, -3}; // Default position
    glm::vec3 rotation{0, 0, 0};   // Default rotation
    glm::vec3 scale{0.01, 0.01, 0.01}; // Default scale

    // Constructor
    MySphere();

    // Update the sphere's transformation matrix
    bool update(float dTime, Scene &scene) override;

    // Render the sphere
    void render(Scene &scene) override;
};
