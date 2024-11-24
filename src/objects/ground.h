// Ground.h
#ifndef GROUND_H
#define GROUND_H

#include <ppgso/ppgso.h>
#include "../scene.cpp"
#include "src/renderable.h"

class Ground final : public Renderable {
private:
    glm::mat4 modelMatrix{1.0f};

    // Static resources
    std::unique_ptr<ppgso::Mesh> mesh;
    std::unique_ptr<ppgso::Texture> texture;

public:
    glm::vec3 position{7.0f, 0.0f, -4.0f};
    glm::vec3 scale{50.0f, 50.0f, 1.0f};
    std::vector<std::unique_ptr<Renderable>> children;  // Scene hierarchy children objects

    // Constructor
    Ground();

    // Override virtual functions from Renderable
    bool update(float dTime, Scene &scene) override;
    void render(Scene &scene) override;
};

#endif // GROUND_H
