// Ground.cpp
#include "ground.h"
#include <iostream> // For std::cerr

Ground::Ground() {
    // Initialize texture if not already initialized
    if (!texture) {
        auto image = ppgso::image::loadBMP("ground.bmp");
        if (image.width == 0 || image.height == 0) {
            std::cerr << "Failed to load image: ground.bmp" << std::endl;
            return;
        }
        texture = std::make_unique<ppgso::Texture>(std::move(image));
    }

    // Initialize mesh if not already initialized
    if (!mesh) {
        mesh = std::make_unique<ppgso::Mesh>("quad.obj");
    }
}

bool Ground::update(float dTime, Scene &scene) {
    // Reset model matrix
    modelMatrix = glm::mat4{1.0f};

    // Update all children with the current model matrix
    for (auto &child : children) {
        child->update_child(dTime, scene, modelMatrix);
    }

    // Apply transformations
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3{-1.0f, 0.0f, 0.0f});
    modelMatrix = glm::scale(modelMatrix, scale);

    return true;
}

void Ground::render(Scene &scene) {
    // Use the scene's shader
    scene.shader->use();

    // Set shader uniforms
    scene.shader->setUniform("ModelMatrix", modelMatrix);
    scene.shader->setUniform("ViewMatrix", scene.camera->viewMatrix);
    scene.shader->setUniform("ProjectionMatrix", scene.camera->perspective);

    // Bind and set the texture
    scene.shader->setUniform("Texture", *texture);

    // Render the mesh
    mesh->render();

    // Render all children
    for (auto &child : children) {
        child->render(scene);
    }
}
