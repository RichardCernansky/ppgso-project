//
// Created by Bruno Kristián on 27/10/2024.
//

#include "stone.h"

std::unique_ptr<ppgso::Mesh> Stone::mesh;
std::unique_ptr<ppgso::Texture> Stone::texture;

// Constructor
Stone::Stone() {
    if (!texture) {
        auto image = ppgso::image::loadBMP("rock.bmp");  // Assuming a texture for boar
        if (image.width == 0 || image.height == 0) {
            std::cerr << "Failed to load texture: rock.bmp" << std::endl;
            return;
        }
        texture = std::make_unique<ppgso::Texture>(std::move(image));
    }

    if (!mesh) {
        mesh = std::make_unique<ppgso::Mesh>("rock.obj");  // Load the boar model
        if (!mesh) {
            std::cerr << "Failed to load mesh: rock" << std::endl;
        }
    }
}

// Update method
bool Stone::update(float dTime, Scene &scene) {

    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(0.0f), glm::vec3{-1, 0, 0});
    modelMatrix = glm::scale(modelMatrix, scale);

    return true;
}

void Stone::render(Scene &scene) {

    // Use the shader program
    scene.shader->use();

    // Set shader uniforms
    scene.shader->setUniform("ModelMatrix", modelMatrix);
    scene.shader->setUniform("ViewMatrix", scene.camera->viewMatrix);
    scene.shader->setUniform("ProjectionMatrix", scene.camera->perspective);
    scene.shader->setUniform("Texture", *texture);

    mesh->render();
}