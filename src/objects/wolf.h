#ifndef WOLF_H
#define WOLF_H

#include <ppgso/ppgso.h>

#include "../scene.cpp"
#include "../renderable.h"

class Wolf final : public Renderable {

    glm::mat4 modelMatrix{1.0f};
    static std::unique_ptr<ppgso::Mesh> mesh;
    static std::unique_ptr<ppgso::Texture> texture;

public:
    glm::vec3 scale{0.01, 0.01, 0.01};
    glm::vec3 velocity{0.0f, 0.0f, 0.0f};
    glm::vec3 acceleration{0.0f, 0.0f, 0.0f};
    float mass = 1.0f;
    float gravity = 9.8f;
    float dt = 0.016f;

    Wolf();

    int findClosestPigIndex(const Scene &scene) const;
    bool update(float dTime, Scene &scene) override;
    void render(Scene &scene) override;

    const std::vector<std::unique_ptr<Renderable>>& getChildren() const {
        return children;
    }
};
#endif
