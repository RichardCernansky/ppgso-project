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

bool Apple::update(float dTime, Scene &scene) {
    return true;
}

bool Apple::update_child(float dTime, Scene &scene, glm::mat4 ParentModelMatrix) {

    glm::vec3 parentPosition = glm::vec3(ParentModelMatrix[3]);

    dt = dTime;
    glm::vec3 gravity_force = glm::vec3(0.0f, -gravity * mass, 0.0f);
    glm::vec3 total_force = gravity_force + wind_force;

    acceleration = total_force / mass;
    velocity += acceleration * dt;
    position += velocity * dt;

    // Ground collision detection and response
    if (position.y < 0.1f) {
        position.y = 0.1f;
        velocity.y = -velocity.y * restitution;

        if (abs(velocity.y) < 0.1f) {

            position = glm::vec3(-1, 3, -1);
            velocity =  glm::vec3(0, 0, 0);
            acceleration = glm::vec3(0, 0, 0);
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