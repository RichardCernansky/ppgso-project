#include <ppgso/ppgso.h>
#include "../scene.cpp"
#include "src/renderable.h"

class Gas final : public Renderable {
private:
    glm::mat4 modelMatrix{1.0f};
    glm::mat4 parentPos{0};

    static std::unique_ptr<ppgso::Mesh> mesh;
    static std::unique_ptr<ppgso::Texture> texture;

public:
    std::vector<std::unique_ptr<Renderable>> children;
    glm::vec3 position{0, 0, -0.7};
    glm::vec3 scale{0.008, 0.008, 0.008};

    Gas();

    bool update(float dTime, Scene &scene) override;
    bool update_child(float dTime, Scene &scene, glm::mat4 ParetModelMatrix) override;
    void render(Scene &scene) override;
};