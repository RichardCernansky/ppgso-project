#include <ppgso/ppgso.h>
#include "../scene.cpp"
#include "src/renderable.h"

class HorseFly final : public Renderable {
private:
    glm::mat4 modelMatrix{1.0f};
    glm::mat4 parentPos{0};
    static std::unique_ptr<ppgso::Mesh> mesh;
    static std::unique_ptr<ppgso::Texture> texture;

public:
    glm::vec3 position{0, 2, -1.5};
    glm::vec3 scale{0.05, 0.05, 0.05};
    float timeInState = 0.0f;
    bool goingUp = true;

    HorseFly();

    bool update(float dTime, Scene &scene) override;
    bool update_child(float dTime, Scene &scene, glm::mat4 ParetModelMatrix) override;
    void render(Scene &scene) override;
};