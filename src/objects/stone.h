#include <ppgso/ppgso.h>
#include "../scene.cpp"
#include "src/renderable.h"

#ifndef STONE_H
#define STONE_H
class Stone final : public Renderable {
private:
    glm::mat4 modelMatrix{1.0f};
    static std::unique_ptr<ppgso::Mesh> mesh;
    static std::unique_ptr<ppgso::Texture> texture;

public:
    glm::vec3 position{0.5f, 0.0f, -2.0f};
    glm::vec3 scale{0.04, 0.04, 0.04};
    float radius = 20*scale.x;
    std::vector<std::unique_ptr<Renderable>> children;


    Stone();
    bool update(float dTime, Scene &scene) override;
    void render(Scene &scene) override;
    bool update_child(float dTime, Scene &scene, glm::mat4 parentModelMatrix) override;
};
#endif
