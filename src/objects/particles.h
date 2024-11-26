#ifndef PARTICLES_H
#define PARTICLES_H

#include <ppgso/ppgso.h>
#include "../scene.cpp"
#include "../renderable.h"

class Particles : public Renderable {
    glm::mat4 modelMatrix{1.0f};
    std::vector<std::unique_ptr<Renderable>> children;
    glm::mat4 parentPos{0};

    static std::unique_ptr<ppgso::Mesh> mesh;
    static std::unique_ptr<ppgso::Texture> texture;

public:
    glm::vec3 position;
    float scale_one;
    glm::vec3 scale;
    glm::vec3 speed;
    float age = 0;
    float transparency = 0.5f;

    Particles();

    bool update(float dTime, Scene &scene) override;
    bool update_child(float dTime, Scene &scene, glm::mat4 ParentModelMatrix);
    void render(Scene &scene) override;
    void reset();
    bool isTransparent() const override;
    glm::vec3 getGlobalPosition() const;
    float calculateDepthFromCamera(const glm::vec3& cameraPosition) const override;
};

#endif
