//
// Created by Bruno Kristian on 18/10/2024.
//
// apple.h

#ifndef APPLE_H
#define APPLE_H

#include <ppgso/ppgso.h>
#include "../scene.cpp"
#include "../renderable.h"

class Apple final : public Renderable {
    // Transformation matrices
    glm::mat4 modelMatrix{1.0f};


    // Static resources shared by all apples
    static std::unique_ptr<ppgso::Mesh> mesh;
    static std::unique_ptr<ppgso::Texture> texture;

public:
    glm::vec3 scale{0.1f, 0.1f, 0.1}; // Adjust as needed
    glm::vec3 position{0, 0, 0};
    glm::vec3 constructorPosition{0, 0, 0};
    glm::vec3 velocity{0.0f, 0.0f, 0.0f};
    glm::vec3 acceleration{0.0f, 0.0f, 0.0f};
    float mass = 0.2f;
    float gravity = 9.8f;
    glm::vec3 wind_force{0.5f, 0.0f, 0.0f};
    float dt = 0.016f;
    float restitution = 0.3f;

    bool isFalling = false;
    float respawnTime  = 0.0f;
    float elapsedTime = 0.0f;

    //float timeSinceStopped = 0.0f;
    bool landed = false;
    float timeSinceAttached = 0.0f;
    float timeBeforeFall = 0.0f;

    Apple();

    void render(Scene &scene) override;
    bool update(float dTime, Scene &scene) override;
    bool update_child(float dTime, Scene &scene, glm::mat4 ParentModelMatrix) override;


};
#endif // APPLE_H
