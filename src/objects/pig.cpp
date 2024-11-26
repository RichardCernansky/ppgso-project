#include "pig.h"
#include "src/generator.h"
#include <random>
#include "wolf.h"
#include "src/globals.h"

std::unique_ptr<ppgso::Mesh> Pig::mesh;
std::unique_ptr<ppgso::Texture> Pig::texture;

Pig::Pig() {
    position = glm::vec3 {0, 0, -5};
    initialPosition = position;
    if (!texture) {
        auto image = ppgso::image::loadBMP("pink_prasa2.bmp");
        if (image.width == 0 || image.height == 0) {
            std::cerr << "Failed to load texture: boar_texture.bmp" << std::endl;
            return;
        }
        texture = std::make_unique<ppgso::Texture>(std::move(image));
    }

    if (!mesh) {
        mesh = std::make_unique<ppgso::Mesh>("pig.obj");
        if (!mesh) {
            std::cerr << "Failed to load mesh: boar.obj" << std::endl;
        }
    }
}

bool Pig::isCollided(const Scene &scene) const {
    for (const auto& object : scene.objects) {
        if (typeid(*object) == typeid(Wolf)) {
            float distance = glm::distance(this->getPosition(), object->getPosition());
            if (distance < wolfCollisionThreshold) {
                return true; // Collision detected
            }
        }
    }

    return false;
}

void Pig::run_off() {
    maxRunDuration = randomFloat(5.0f, 10.0f);
    float randomAngle = glm::radians(randomFloat(0.0f, 360.0f));
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), randomAngle, glm::vec3(0.0f, 1.0f, 0.0f));
    runDirection = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f))); // Rotate forward direction (0, 0, 1)

    isRunningOff = true;
    runDuration = 0.0f;
    currentSpeed = 0.0f;
}


bool Pig::update(float dTime, Scene &scene) {
    float distanceFromInitial = glm::distance(position, initialPosition);

    if (distanceFromInitial > 20.0f) {
        runDirection = glm::normalize(initialPosition - position);
        currentSpeed = 5.0f;

        position += runDirection * currentSpeed * dTime;

        modelMatrix = glm::mat4{1.0f};
        modelMatrix = glm::translate(modelMatrix, position);
        modelMatrix *= rotateToFaceDirection({0, 0, 1}, runDirection);
        modelMatrix = glm::scale(modelMatrix, scale);

        return true;
    }

    if (isCollided(scene) && !isRunningOff) {
        run_off();
    }

    if (isRunningOff) {
        runDuration += dTime;

        float accelerationFactor = glm::min(1.0f, runDuration / maxRunDuration);
        currentSpeed = accelerationFactor * 10.0f;
        position += runDirection * currentSpeed * dTime;

        modelMatrix = glm::mat4{1.0f};
        modelMatrix = glm::translate(modelMatrix, position);
        modelMatrix *= rotateToFaceDirection({0, 0, 1}, runDirection);
        modelMatrix = glm::scale(modelMatrix, scale);

        if (runDuration >= maxRunDuration) {
            isRunningOff = false;
        }
    } else {
        timeInState += dTime;

        if (timeInState >= changeDirectionTime) {
            timeInState = 0.0f;
            changeDirectionTime = randomFloat(2.0f, 5.0f);
            float randomAngle = randomFloat(0.0f, 360.0f);
            glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(randomAngle), glm::vec3(0.0f, 1.0f, 0.0f));
            globalDirection = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f)));
        }

        position += globalDirection * 1.0f * dTime;

        modelMatrix = glm::mat4{1.0f};
        modelMatrix = glm::translate(modelMatrix, position);
        modelMatrix *= rotateToFaceDirection({0, 0, 1}, globalDirection);
        modelMatrix = glm::scale(modelMatrix, scale);
    }

    for (auto& child : children) {
        child->update_child(dTime, scene, modelMatrix);
    }

    return true;
}


void Pig::render(Scene &scene) {
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

    for (auto& child : children) {
        child->render(scene);
    }
}
