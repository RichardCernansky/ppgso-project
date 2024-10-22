#include <ppgso/ppgso.h>
#include "../scene.cpp"
#include "src/renderable.h"

class Gas final : public Renderable {
private:
    glm::mat4 modelMatrix{1.0f};  // Model transformation matrix
    glm::mat4 parentPos{0};
    // Static resources (shared among all pigs)
    static std::unique_ptr<ppgso::Mesh> mesh;
    static std::unique_ptr<ppgso::Texture> texture;

public:
    std::vector<std::unique_ptr<Renderable>> children;  // scene hierarchy children objects
    // Pig position and scale
    glm::vec3 position{0, 0, -0.7};
    glm::vec3 scale{0.008, 0.008, 0.008};     // Default scale
    Gas();  // Constructor

    bool update(float dTime, Scene &scene) override;
    // Update and render methods
    bool update_child(float dTime, Scene &scene, glm::mat4 ParetModelMatrix) override;
    void render(Scene &scene) override;
    bool isTransparent() const override { return true; }
    glm::vec3 getPosition() const override { return position; }
};