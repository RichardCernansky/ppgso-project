// bee.cpp
#include "bee.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "src/scene.cpp"
#include "src/globals.h"

// Static resources (shared among all instances of Bee)
std::unique_ptr<ppgso::Mesh> Bee::mesh;
std::unique_ptr<ppgso::Texture> Bee::texture;

// Constructor
Bee::Bee() {
    position = {0, 0,-2.5};
    if (!texture) {
        auto image = ppgso::image::loadBMP("bee2.bmp");
        if (image.width == 0 || image.height == 0) {
            std::cerr << "Failed to load texture: bee2.bmp" << std::endl;
            return;
        }
        texture = std::make_unique<ppgso::Texture>(std::move(image));
    }

    if (!mesh) {
        mesh = std::make_unique<ppgso::Mesh>("lowBee.obj");
        if (!mesh) {
            std::cerr << "Failed to load mesh: lowBee.obj" << std::endl;
        }
    }

    // Define waypoints for the bee's flight path with both position and rotation
    waypoints = {
        {{1.0f, 0.5f, 2.0f}, {0.0f, 0.0f, 0.0f}},
        {{0.0f, 1.0f, 1.5f}, {0.0f, 90.0f, 0.0f}},
        {{0.0f, 1.0f, 0.0f}, {0.0f, 180.0f, 0.0f}},       // Position and rotation
        {{1.0f, 1.5f, 0.5f}, {0.0f, 80.0f, 0.0f}},
        {{2.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}
    };

    currentWaypoint = 0;
    journeyProgress = 0.0f;
}

// Update function
bool Bee::update(float dTime, Scene &scene) {
    // Increase the journey progress by time elapsed times speed
    journeyProgress += dTime * speed;

    // Get current and next keyframes
    Keyframe start = waypoints[currentWaypoint];
    Keyframe end = waypoints[(currentWaypoint + 1) % waypoints.size()];

    // Interpolate position between start and end keyframes
    auto j_position = glm::mix(start.position, end.position, journeyProgress);

    // Interpolate rotation between start and end keyframes
    glm::vec3 interpolatedRotation = glm::mix(start.rotation, end.rotation, journeyProgress);

    // Check if the bee reached the next waypoint
    if (journeyProgress >= 1.0f) {
        journeyProgress = 0.0f;  // Reset progress for the next segment
        currentWaypoint = (currentWaypoint + 1) % waypoints.size();  // Move to the next waypoint
    }

    // Create model transformation matrix for the bee
    modelMatrix = glm::mat4{1.0f};
    modelMatrix = glm::translate(modelMatrix, j_position);
    modelMatrix = glm::translate(modelMatrix, position);


    // Apply rotation based on interpolated rotation values
    modelMatrix *= glm::yawPitchRoll(
        glm::radians(interpolatedRotation.y),  // Yaw (rotation around Y-axis)
        glm::radians(interpolatedRotation.x),  // Pitch (rotation around X-axis)
        glm::radians(interpolatedRotation.z)   // Roll (rotation around Z-axis)
    );

    // Scale down the bee model if needed
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));

    return true;
}

// Render function
void Bee::render(Scene &scene) {
    // Use the shadow projection matrix
    glm::mat4 shadowMatrix = calculateShadowMatrix(moonLight_position, glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));

    // Render the shadow
    scene.colorShader->use();
    scene.colorShader->setUniform("ModelMatrix", shadowMatrix * modelMatrix);
    scene.colorShader->setUniform("ViewMatrix", scene.camera->viewMatrix);
    scene.colorShader->setUniform("ProjectionMatrix", scene.camera->perspective);

    // Render the pig's shadow as a black silhouette
    glDisable(GL_DEPTH_TEST); // Prevent z-fighting
    scene.colorShader->setUniform("Color", glm::vec3(0.0f, 0.0f, 0.0f)); // Black shadow
    mesh->render();
    glEnable(GL_DEPTH_TEST);

    // Render the pig
    scene.shader->use();
    scene.shader->setUniform("ModelMatrix", modelMatrix);
    scene.shader->setUniform("ViewMatrix", scene.camera->viewMatrix);
    scene.shader->setUniform("ProjectionMatrix", scene.camera->perspective);
    scene.shader->setUniform("Texture", *texture);

    mesh->render();
}
