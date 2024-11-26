#include <ppgso/ppgso.h>
#include "../scene.cpp"
#include "src/renderable.h"

class Pig final : public Renderable {
private:
    glm::mat4 modelMatrix{1.0f};
    static std::unique_ptr<ppgso::Mesh> mesh;
    static std::unique_ptr<ppgso::Texture> texture;

public:
    glm::vec3 scale{1, 1, 1};
    glm::vec3 globalDirection{0,0,1};
    glm::vec3 initialPosition{0, 0, 0};
    float timeInState = 0.0f;
    float changeDirectionTime = 0.0f;
    float wolfCollisionThreshold = 2.0f;
    float runDuration = 0.0f;
    float maxRunDuration = 4.0f;
    float currentSpeed = 0.0f;
    glm::vec3 runDirection;
    bool isRunningOff = false;

    Pig();

    bool update(float dTime, Scene &scene) override;
    void render(Scene &scene) override;
    bool isCollided(const Scene &scene) const;
    void run_off();
};

