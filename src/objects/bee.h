#pragma once

#include <ppgso/ppgso.h>
#include "../renderable.h"
#include <vector>
#include <glm/glm.hpp>

class Bee : public Renderable {
private:
    struct Keyframe {
        glm::vec3 position;
        glm::vec3 rotation;
    };

    static std::unique_ptr<ppgso::Mesh> mesh;
    static std::unique_ptr<ppgso::Texture> texture;

    std::vector<Keyframe> waypoints;

    int currentWaypoint = 0;
    float journeyProgress = 0.0f;
    float speed = 1.0f;

    glm::mat4 modelMatrix{1.0f};

public:
    Bee();

    bool update(float dTime, Scene &scene) override;
    void render(Scene &scene) override;
};
