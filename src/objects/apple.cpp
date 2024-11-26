#include "apple.h"
#include "src/generator.h"

std::unique_ptr<ppgso::Mesh> Apple::mesh;
std::unique_ptr<ppgso::Texture> Apple::texture;

Apple::Apple() {
    float baseX = 0.0f, baseY = 3.0f, baseZ = 0.0f;
    float offset = 0.5;
    constructorPosition = glm::vec3{
        baseX + randomFloat(-offset, offset),
        baseY + randomFloat(-offset, offset),
        baseZ + randomFloat(-offset, offset)
    };
    position = constructorPosition;

    if (!texture) {
        auto image = ppgso::image::loadBMP("apple.bmp");
        if (image.width == 0 || image.height == 0) {
            std::cerr << "Failed to load texture: apple.bmp" << std::endl;
            return;
        }
        texture = std::make_unique<ppgso::Texture>(std::move(image));
    }

    if (!mesh) {
        mesh = std::make_unique<ppgso::Mesh>("apple.obj");
        if (!mesh) {
            std::cerr << "Failed to load mesh: apple.obj" << std::endl;
        }
    }
}

bool Apple::update(float dTime, Scene &scene) {
    return true;
}

bool Apple::update_child(float dTime, Scene &scene, glm::mat4 ParentModelMatrix) {
    dt = dTime;

    if (isFalling) {
        glm::vec3 gravity_force = glm::vec3(0.0f, -gravity * mass, 0.0f);
        glm::vec3 total_force = gravity_force + wind_force;

        acceleration = total_force / mass;
        velocity += acceleration * dt;
        position += velocity * dt;

        if (position.y < 0.1f) {
            position.y = 0.1f;
            velocity.y = -velocity.y * restitution;

            if (abs(velocity.y) < 0.1f) {
                isFalling = false;
                respawnTime = randomFloat(2.0f, 5.0f);
                elapsedTime = 0.0f;
                position = constructorPosition;
                velocity = glm::vec3(0, 0, 0);
                acceleration = glm::vec3(0, 0, 0);
            }
        }
    } else {
        elapsedTime += dt;

        if (elapsedTime >= respawnTime) {
            isFalling = true;
        }
    }

    glm::mat4 localModelMatrix = glm::translate(glm::mat4(1.0f), position);
    localModelMatrix = glm::scale(localModelMatrix, scale);
    modelMatrix = ParentModelMatrix * localModelMatrix;

    return true;
}

void Apple::render(Scene &scene) {
    scene.shader->use();

    scene.shader->setUniform("ModelMatrix", modelMatrix);
    scene.shader->setUniform("ViewMatrix", scene.camera->viewMatrix);
    scene.shader->setUniform("ProjectionMatrix", scene.camera->perspective);

    if (texture) {
        scene.shader->setUniform("Texture", *texture);
    }

    if (mesh) {
        mesh->render();
    }
}
