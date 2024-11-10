// bee.h
#pragma once

#include <ppgso/ppgso.h>
#include "../renderable.h"
#include <vector>
#include <glm/glm.hpp>

class Bee : public Renderable {
private:
    // Struct to represent each waypoint with position and rotation
    struct Keyframe {
        glm::vec3 position;
        glm::vec3 rotation;
    };

    // Static resources shared among all instances of Bee
    static std::unique_ptr<ppgso::Mesh> mesh;
    static std::unique_ptr<ppgso::Texture> texture;

    // Waypoints for the bee’s flight path
    std::vector<Keyframe> waypoints;

    // Movement and transformation state
    int currentWaypoint = 0;  // Index of the current waypoint
    float journeyProgress = 0.0f;  // Interpolation progress between waypoints
    float speed = 1.0f;  // Flying speed of the bee

    glm::mat4 modelMatrix{1.0f};  // Transformation matrix for rendering

public:
    // Constructor
    Bee();

    // Update function to handle the bee’s movement along waypoints
    bool update(float dTime, Scene &scene) override;

    // Render function to draw the bee
    void render(Scene &scene) override;
};
