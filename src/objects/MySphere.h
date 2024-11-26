#include <ppgso/ppgso.h>
#include "../scene.cpp"
#include "../renderable.h"

class MySphere final : public Renderable {
    static std::unique_ptr<ppgso::Mesh> mesh;
    static std::unique_ptr<ppgso::Texture> texture;

public:
    glm::mat4 modelMatrix{1.0f};
    glm::vec3 position{-50, 20, 0};
    glm::vec3 rotation{0, 0, 0};
    glm::vec3 scale{1, 1, 1};

    MySphere();

    bool update(float dTime, Scene &scene) override;
    void render(Scene &scene) override;
};
