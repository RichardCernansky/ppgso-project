#include "apple.h"

// Static resources (shared among all instances of Pig)
std::unique_ptr<ppgso::Mesh> Apple::mesh;
std::unique_ptr<ppgso::Texture> Apple::texture;

// Constructor
Apple::Apple() {

    if (!texture) {
        auto image = ppgso::image::loadBMP("apple.bmp");  // Assuming a texture for boar
        if (image.width == 0 || image.height == 0) {
            std::cerr << "Failed to load texture: boar_texture.bmp" << std::endl;
            return;
        }
        texture = std::make_unique<ppgso::Texture>(std::move(image));
    }


    if (!mesh) {
        mesh = std::make_unique<ppgso::Mesh>("apple.obj");  // Load the boar model
        if (!mesh) {
            std::cerr << "Failed to load mesh: apple.obj" << std::endl;
        }
    }
}


// Update method
bool Apple::update(float dTime, Scene &scene) {
    //if (!landed) {
        // Update the time step based on delta time
        dt = dTime;

        // Calculate the forces acting on the apple
        glm::vec3 gravity_force = glm::vec3(0.0f, -gravity * mass, 0.0f);
        glm::vec3 total_force = gravity_force + wind_force;

        // Calculate acceleration (F = ma => a = F / m)
        acceleration = total_force / mass;

        // Update velocity
        velocity += acceleration * dt;

        // Update position
        position += velocity * dt;

        if (position.y < 0.2) {
            position.y = 0.2;

            // Apply the coefficient of restitution to simulate energy loss
            velocity.y = -velocity.y * restitution;

            // Optional: Apply a small threshold to stop bouncing when velocity is too low
            if (abs(velocity.y) < 0.1f) {
                velocity.y = 0.0f; // Stop bouncing
                position = glm::vec3(3, 10, 0);
                velocity = glm::vec3(0, 0, 0);
                acceleration = glm::vec3(0, 0, 0);
                //landed = true;
            }
        }

        // Update the model matrix
        modelMatrix = glm::translate(glm::mat4(1.0f), position);
        modelMatrix = glm::scale(modelMatrix, scale);
    //}
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