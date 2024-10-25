//
// Created by Richard Čerňanský on 12/10/2024.
//

// pig.cpp
#include "pig.h"
#include "src/generator.h"
#include <random>

// Static resources (shared among all instances of Pig)
std::unique_ptr<ppgso::Mesh> Pig::mesh;
std::unique_ptr<ppgso::Texture> Pig::texture;

// Constructor
Pig::Pig() {
    position = glm::vec3 {5, 0, -10};
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


// Update method
bool Pig::update(float dTime, Scene &scene) {
    timeInState += dTime;

    if (timeInState <= 5) {
        position += globalDirection * 1.0f * dTime;
        modelMatrix = glm::mat4{1.0f};
        modelMatrix = glm::translate(modelMatrix, position);
        modelMatrix *= rotateToFaceDirection({0,0,1}, globalDirection);
        modelMatrix = glm::scale(modelMatrix, scale);
    } else {
        float angle = glm::radians(90.0f);
        modelMatrix = glm::mat4{1.0f};
        modelMatrix = glm::rotate(modelMatrix, angle, glm::vec3(0.0f, 1.0f, 0.0f));
        modelMatrix = glm::scale(modelMatrix, scale);
        timeInState = 0.0f;
        globalDirection = glm::vec4(globalDirection, 0.0f) * modelMatrix;
    }


    for (auto& child : children) {
        child->update_child(dTime,scene, modelMatrix);
    }

    return true;
}

// Render method
void Pig::render(Scene &scene) {
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

