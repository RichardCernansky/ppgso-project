//
// Created by Bruno Kristián on 23/10/2024.
//

#ifndef PARTICLES_H
#define PARTICLES_H

#include <ppgso/ppgso.h>
#include "../scene.cpp"

class Particles final : public Renderable {
    glm::mat4 modelMatrix{1.0f};
    std::vector<std::unique_ptr<Renderable>> children;
    glm::mat4 parentPos{0};

    static std::unique_ptr<ppgso::Mesh> mesh;
    static std::unique_ptr<ppgso::Texture> texture;

public:
    glm::vec3 scale{0.1, 0.1, 0.1}; // Adjust as needed
    glm::vec3 position{0, 0.5,-0.8};

    Particles();

    void render(Scene &scene) override;
    bool update(float dTime, Scene &scene) override;
    bool update_child(float dTime, Scene &scene, glm::mat4 ParetModelMatrix) override;

};
#endif //PARTICLES_H
