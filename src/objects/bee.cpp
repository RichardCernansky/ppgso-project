#include "bee.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "src/scene.cpp"
#include "src/globals.h"

std::unique_ptr<ppgso::Mesh> Bee::mesh;
std::unique_ptr<ppgso::Texture> Bee::texture;

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

bool Bee::update(float dTime, Scene &scene) {
    journeyProgress += dTime * speed;

    Keyframe start = waypoints[currentWaypoint];
    Keyframe end = waypoints[(currentWaypoint + 1) % waypoints.size()];

    auto j_position = glm::mix(start.position, end.position, journeyProgress);

    glm::vec3 interpolatedRotation = glm::mix(start.rotation, end.rotation, journeyProgress);

    if (journeyProgress >= 1.0f) {
        journeyProgress = 0.0f;  // Reset progress for the next segment
        currentWaypoint = (currentWaypoint + 1) % waypoints.size();  // Move to the next waypoint
    }

    modelMatrix = glm::mat4{1.0f};
    modelMatrix = glm::translate(modelMatrix, j_position);
    modelMatrix = glm::translate(modelMatrix, position);


    modelMatrix *= glm::yawPitchRoll(
        glm::radians(interpolatedRotation.y),  // Yaw (rotation around Y-axis)
        glm::radians(interpolatedRotation.x),  // Pitch (rotation around X-axis)
        glm::radians(interpolatedRotation.z)   // Roll (rotation around Z-axis)
    );

    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));

    return true;
}

void Bee::render(Scene &scene) {
    glm::mat4 shadowMatrix = calculateShadowMatrix(moonLight_position, glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
    shadowMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, shadow_y_offset, 0.0f)) * shadowMatrix;

    scene.colorShader->use();
    scene.colorShader->setUniform("ModelMatrix", shadowMatrix * modelMatrix);
    scene.colorShader->setUniform("ViewMatrix", scene.camera->viewMatrix);
    scene.colorShader->setUniform("ProjectionMatrix", scene.camera->perspective);
    scene.colorShader->setUniform("Color", glm::vec3(0.0f, 0.0f, 0.0f)); // Black shadow

    mesh->render();

    scene.shader->use();
    scene.shader->setUniform("ModelMatrix", modelMatrix);
    scene.shader->setUniform("ViewMatrix", scene.camera->viewMatrix);
    scene.shader->setUniform("ProjectionMatrix", scene.camera->perspective);
    scene.shader->setUniform("Texture", *texture);

    mesh->render();
}
