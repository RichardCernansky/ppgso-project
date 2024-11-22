#include <ppgso/ppgso.h>
#include "../scene.cpp"
#include "../renderable.h"

class MySphere final : public Renderable {

    // Static resources
    static std::unique_ptr<ppgso::Mesh> mesh;
    static std::unique_ptr<ppgso::Texture> texture;

public:
    glm::mat4 modelMatrix{1.0f};
    // Position, rotation, and scale of the sphere
    glm::vec3 position{-50, 20, 0}; // Default position
    glm::vec3 rotation{0, 0, 0};   // Default rotation
    glm::vec3 scale{1, 1, 1}; // Default scale

    // Constructor
    MySphere();

    // Update the sphere's transformation matrix
    bool update(float dTime, Scene &scene) override;

    // Render the sphere
    void render(Scene &scene) override;
};
