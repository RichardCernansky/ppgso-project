#include "apple.h"
#include "src/generator.h"

// Static resources (shared among all instances of Apple)
std::unique_ptr<ppgso::Mesh> Apple::mesh;
std::unique_ptr<ppgso::Texture> Apple::texture;

// Constructor
Apple::Apple() {
    // Generate a random position with each axis offset by a random value in the range [-1, 1]
    float baseX = 0.0f, baseY = 3.0f, baseZ = 0.0f;
    float offset = 0.5;
    constructorPosition = glm::vec3{
        baseX + randomFloat(-offset, offset),  // Random offset from baseX
        baseY + randomFloat(-offset, offset),  // Random offset from baseY
        baseZ + randomFloat(-offset, offset)   // Random offset from baseZ
    };
    position = constructorPosition;

    // Load texture once and reuse across all instances
    if (!texture) {
        auto image = ppgso::image::loadBMP("apple.bmp");
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

bool Apple::update(float dTime, Scene &scene) {
    return true;
}

bool Apple::update_child(float dTime, Scene &scene, glm::mat4 ParentModelMatrix) {
    dt = dTime;

    // Handle the state of the apple: falling or waiting to respawn
    if (isFalling) {
        // Falling phase
        glm::vec3 gravity_force = glm::vec3(0.0f, -gravity * mass, 0.0f);
        glm::vec3 total_force = gravity_force + wind_force;

        acceleration = total_force / mass;
        velocity += acceleration * dt;
        position += velocity * dt;

        // Ground collision detection and response
        if (position.y < 0.1f) {
            position.y = 0.1f;
            velocity.y = -velocity.y * restitution;

            // When the apple comes to rest, despawn it if it's not bouncing anymore
            if (abs(velocity.y) < 0.1f) {
                // Apple hits the ground and despawns
                isFalling = false;
                respawnTime = randomFloat(2.0f, 5.0f);  // Random delay between 2 and 5 seconds
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

void Apple::render(Scene &scene) {
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
