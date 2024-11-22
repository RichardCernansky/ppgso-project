//
// Created by Richard Čerňanský on 12/10/2024.
//

// pig.cpp
#include "pig.h"
#include "src/generator.h"
#include <random>
#include "wolf.h"
#include "src/globals.h"

// Static resources (shared among all instances of Pig)
std::unique_ptr<ppgso::Mesh> Pig::mesh;
std::unique_ptr<ppgso::Texture> Pig::texture;

// Constructor
Pig::Pig() {
    position = glm::vec3 {0, 0, -5};
    initialPosition = position;
    // Load texture and mesh only once
    if (!texture) {
        auto image = ppgso::image::loadBMP("pink_prasa.bmp");  // Assuming a texture for boar
        if (image.width == 0 || image.height == 0) {
            std::cerr << "Failed to load texture: boar_texture.bmp" << std::endl;
            return;
        }
        texture = std::make_unique<ppgso::Texture>(std::move(image));
    }

    if (!mesh) {
        mesh = std::make_unique<ppgso::Mesh>("pig.obj");  // Load the boar model
        if (!mesh) {
            std::cerr << "Failed to load mesh: boar.obj" << std::endl;
        }
    }
}

bool Pig::isCollided(const Scene &scene) const {
    // Loop through all objects in the scene
    for (const auto& object : scene.objects) {

        // Check if the unique_ptr is pointing to a Wolf using typeid
        if (typeid(*object) == typeid(Wolf)) {
            // Calculate the distance between the pig and the wolf
            float distance = glm::distance(this->getPosition(), object->getPosition());

            // Check if the distance is below the collision threshold
            if (distance < wolfCollisionThreshold) {
                return true; // Collision detected
            }
        }
    }

    // No collision detected
    return false;
}

void Pig::run_off() {
    // Randomly select a new run duration between 3 to 5 seconds
    maxRunDuration = randomFloat(5.0f, 10.0f);

    // Choose a random direction in the XZ plane
    float randomAngle = glm::radians(randomFloat(0.0f, 360.0f));
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), randomAngle, glm::vec3(0.0f, 1.0f, 0.0f));
    runDirection = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f))); // Rotate forward direction (0, 0, 1)

    // Reset the running state
    isRunningOff = true;
    runDuration = 0.0f;
    currentSpeed = 0.0f;
}

// pig.cpp

bool Pig::update(float dTime, Scene &scene) {
    // Calculate distance from initial position
    float distanceFromInitial = glm::distance(position, initialPosition);

    // Check if Pig is more than 30 units away from its initial position
    if (distanceFromInitial > 20.0f) {
        // Set direction toward the initial position
        runDirection = glm::normalize(initialPosition - position);
        currentSpeed = 5.0f; // Set a speed to move back

        // Update position to move toward the initial position
        position += runDirection * currentSpeed * dTime;

        // Update model matrix to face the initial position
        modelMatrix = glm::mat4{1.0f};
        modelMatrix = glm::translate(modelMatrix, position);
        modelMatrix *= rotateToFaceDirection({0, 0, 1}, runDirection);
        modelMatrix = glm::scale(modelMatrix, scale);

        // Skip regular behavior if Pig is returning
        return true;
    }

    // If not returning to initial position, proceed with regular update logic
    if (isCollided(scene) && !isRunningOff) {
        run_off(); // Initiate the run-off state
    }

    if (isRunningOff) {
        // Handle the run-off behavior
        runDuration += dTime;

        // Calculate acceleration based on elapsed time
        float accelerationFactor = glm::min(1.0f, runDuration / maxRunDuration); // Gradually accelerate over the duration
        currentSpeed = accelerationFactor * 10.0f; // Maximum speed the pig can reach

        // Update the pig's position based on the run direction and current speed
        position += runDirection * currentSpeed * dTime;

        // Update the model matrix to reflect the new position and direction
        modelMatrix = glm::mat4{1.0f};
        modelMatrix = glm::translate(modelMatrix, position);
        modelMatrix *= rotateToFaceDirection({0, 0, 1}, runDirection); // Rotate to face the run direction
        modelMatrix = glm::scale(modelMatrix, scale);

        if (runDuration >= maxRunDuration) {
            isRunningOff = false;
        }
    } else {
        // Normal behavior: randomly change direction every 2-5 seconds
        timeInState += dTime;

        // Check if it's time to change direction
        if (timeInState >= changeDirectionTime) {
            // Reset the time in the current state
            timeInState = 0.0f;

            // Randomly pick a new change direction time between 2 and 5 seconds
            changeDirectionTime = randomFloat(2.0f, 5.0f);

            // Randomly select a new direction in the XZ plane
            float randomAngle = randomFloat(0.0f, 360.0f);
            glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(randomAngle), glm::vec3(0.0f, 1.0f, 0.0f));

            // Apply the rotation to the forward direction
            globalDirection = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f)));
        }

        // Move the pig in the current direction
        position += globalDirection * 1.0f * dTime;

        // Update the model matrix to face the new direction
        modelMatrix = glm::mat4{1.0f};
        modelMatrix = glm::translate(modelMatrix, position);
        modelMatrix *= rotateToFaceDirection({0, 0, 1}, globalDirection);
        modelMatrix = glm::scale(modelMatrix, scale);
    }

    // Update children
    for (auto& child : children) {
        child->update_child(dTime, scene, modelMatrix);
    }

    return true;
}


// Render method
void Pig::render(Scene &scene) {
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
    // Use the shader and set the transformation matrices
    scene.shader->use();
    scene.shader->setUniform("ModelMatrix", modelMatrix);
    scene.shader->setUniform("ViewMatrix", scene.camera->viewMatrix);
    scene.shader->setUniform("ProjectionMatrix", scene.camera->perspective);
    scene.shader->setUniform("Texture", *texture);

    // Render the mesh
    mesh->render();

    for (auto& child : children) {
        child->render(scene);
    }
}