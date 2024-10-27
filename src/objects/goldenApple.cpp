#include "goldenApple.h"
#include "src/generator.h"
#include "../objects/stone.h"

// Static resources (shared among all instances of Apple)
std::unique_ptr<ppgso::Mesh> GoldenApple::mesh;
std::unique_ptr<ppgso::Texture> GoldenApple::texture;

// Constructor
GoldenApple::GoldenApple() {
    // Generate a random position with each axis offset by a random value in the range [-1, 1]
    float baseX = -0.0f, baseY = 3.0f, baseZ = -0.0f;
    constructorPosition = glm::vec3{
        baseX + randomFloat(-2.0f, 2.0f),  // Random offset from baseX
        baseY + randomFloat(-2.0f, 2.0f),  // Random offset from baseY
        baseZ + randomFloat(-2.0f, 2.0f)   // Random offset from baseZ
    };
    position = constructorPosition;

    // Load texture once and reuse across all instances
    if (!texture) {
        auto image = ppgso::image::loadBMP("golden_apple.bmp");
        if (image.width == 0 || image.height == 0) {
            std::cerr << "Failed to load texture: apple.bmp" << std::endl;
            return;
        }
        texture = std::make_unique<ppgso::Texture>(std::move(image));
    }

    // Load mesh once and reuse across all instances
    if (!mesh) {
        mesh = std::make_unique<ppgso::Mesh>("apple.obj");
        if (!mesh) {
            std::cerr << "Failed to load mesh: apple.obj" << std::endl;
        }
    }
}


bool GoldenApple::update(float dTime, Scene &scene) {
    return true;
}

bool GoldenApple::update_child(float dTime, Scene &scene, glm::mat4 ParentModelMatrix) {
    dt = dTime;

    // Handle the state of the apple: falling or waiting to respawn
    if (isFalling) {
        // Falling phase
        glm::vec3 gravity_force = glm::vec3(0.0f, -gravity * mass, 0.0f);
        glm::vec3 total_force = gravity_force + wind_force;

        acceleration = total_force / mass;
        velocity += acceleration * dt;
        position += velocity * dt;

        // position of the apple
        for (auto &obj : scene.objects) {
            stone = dynamic_cast<Stone*>(obj.get());
        }

        //convert it into world position
        glm::vec4 localPos = glm::vec4(position, 1.0f);
        glm::vec4 worldPos = modelMatrix * localPos;
        glm::vec3 globalPosition = glm::vec3(worldPos);

        //calculate the difference between the apple and the rock
        glm::vec3 diff = globalPosition - stone->position;
        float distance = glm::length(diff);
        float combinedRadius = radius + stone->radius;

        //if they are touching
        if (distance < combinedRadius) {
            // Physics calculations for collision
            glm::vec3 collisionNormal = glm::normalize(position - stone->position);
            float perpendicularVelocity = glm::dot(velocity, collisionNormal);
            glm::vec3 perpendicularComponent = perpendicularVelocity * collisionNormal;
            glm::vec3 parallelComponent = velocity - perpendicularComponent;

            // Reduce the bounce effect by using a lower restitution for the perpendicular component
            float bounceRestitution = restitution * 0.7f;  // Lower than the original restitution
            perpendicularComponent = -perpendicularComponent * bounceRestitution;

            // Apply gravity and friction along the surface for sliding effect
            glm::vec3 gravityAlongSurface = gravity * collisionNormal;
            parallelComponent += gravityAlongSurface * 0.1f;  // Control sliding effect
            float friction = 0.98f;  // Friction for sliding resistance
            parallelComponent *= friction;
            velocity = perpendicularComponent + parallelComponent;
            position = stone->position + collisionNormal * combinedRadius;

            //revert the position back to local
            glm::vec3 referencePosition{10.0f, 0.0f, -5.0f};
            position = position - referencePosition;
        }


        if (position.y < 0.1f) {
            position.y = 0.1f + abs(velocity.y * 0.01f);
            float variedRestitution = restitution * randomFloat(0.9f, 1.1f);
            velocity.y = -velocity.y * variedRestitution;
            float dampingFactor = 0.98f;
            velocity *= dampingFactor;

            if (abs(velocity.y) < 0.1f) {
                isFalling = false;
                respawnTime = randomFloat(1.0f, 2.0f);  // Random delay between 1 and 2 seconds
                elapsedTime = 0.0f;  // Reset elapsed time for the respawn delay

                position = constructorPosition;
                velocity = glm::vec3(0, 0, 0);
                acceleration = glm::vec3(0, 0, 0);
            }
        }




    } else {
        // Waiting for respawn
        elapsedTime += dt;

        // If enough time has passed, reset apple's position and start falling again
        if (elapsedTime >= respawnTime) {
            isFalling = true;  // Set back to falling state
        }
    }

    // Update the model matrix
    glm::mat4 localModelMatrix = glm::translate(glm::mat4(1.0f), position);
    localModelMatrix = glm::scale(localModelMatrix, scale);

    // Combine with parent's model matrix
    modelMatrix = ParentModelMatrix * localModelMatrix;

    return true;
}


void GoldenApple::render(Scene &scene) {
    // Use the shader program
    scene.shader->use();

    // Set shader uniforms
    scene.shader->setUniform("ModelMatrix", modelMatrix);
    scene.shader->setUniform("ViewMatrix", scene.camera->viewMatrix);
    scene.shader->setUniform("ProjectionMatrix", scene.camera->perspective);
    if (texture) {
        scene.shader->setUniform("Texture", *texture);
    }

    // Render the mesh
    if (mesh) {
        mesh->render();
    }
}
