#include <ppgso/ppgso.h>
#include "../scene.cpp"
#include "src/renderable.h"

class Gas final : public Renderable {
private:
    glm::mat4 modelMatrix{1.0f};  // Model transformation matrix

    // Static resources (shared among all pigs)
    static std::unique_ptr<ppgso::Mesh> mesh;
    static std::unique_ptr<ppgso::Texture> texture;

public:
    // Pig position and scale
    glm::vec3 position{0, 0, 0};
    glm::vec3 scale{0.005, 0.005, 0.005};     // Default scale

    Gas();  // Constructor

    // Update and render methods
    bool update(float dTime, Scene &scene) override;
    void render(Scene &scene) override;
};