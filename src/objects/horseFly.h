#include <ppgso/ppgso.h>
#include "../scene.cpp"
#include "src/renderable.h"

class HorseFly final : public Renderable {
private:
    glm::mat4 modelMatrix{1.0f};  // Model transformation matrix
    glm::mat4 parentPos{0};
    // Static resources (shared among all pigs)
    static std::unique_ptr<ppgso::Mesh> mesh;
    static std::unique_ptr<ppgso::Texture> texture;

public:
    // Pig position and scale
    glm::vec3 position{0, 2, -1.5};
    glm::vec3 scale{0.05, 0.05, 0.05};     // Default scale
    float timeInState = 0.0f; // tracks time spent in the current direction
    bool goingUp = true;      // indicates whether the HorseFly is moving up or down
    HorseFly();  // Constructor

    bool update(float dTime, Scene &scene) override;
    // Update and render methods
    bool update_child(float dTime, Scene &scene, glm::mat4 ParetModelMatrix) override;
    void render(Scene &scene) override;
};