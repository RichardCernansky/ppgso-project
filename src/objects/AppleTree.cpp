//
// Created by Bruno Kristián on 18/10/2024.
//

#include "AppleTree.h"

std::unique_ptr<ppgso::Mesh> AppleTree::mesh;
std::unique_ptr<ppgso::Texture> AppleTree::texture;

// Constructor
AppleTree::AppleTree() {
    if (!texture) {
        auto image = ppgso::image::loadBMP("apple_tree.bmp");  // Assuming a texture for boar
        if (image.width == 0 || image.height == 0) {
            std::cerr << "Failed to load texture: apple_tree_texture.bmp" << std::endl;
            return;
        }
        texture = std::make_unique<ppgso::Texture>(std::move(image));
    }

    if (!mesh) {
        mesh = std::make_unique<ppgso::Mesh>("apple_tree.obj");  // Load the boar model
        if (!mesh) {
            std::cerr << "Failed to load mesh: apple_tree" << std::endl;
        }
    }
}

// Update method
bool AppleTree::update(float dTime, Scene &scene) {

    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(0.0f), glm::vec3{-1, 0, 0});
    modelMatrix = glm::scale(modelMatrix, scale);

    for (auto& child : children) {
        child->update_child(dTime, scene, modelMatrix);
    }
    return true;
}

void AppleTree::render(Scene &scene) {

    // Use the shader program
    scene.shader->use();

    // Set shader uniforms
    scene.shader->setUniform("ModelMatrix", modelMatrix);
    scene.shader->setUniform("ViewMatrix", scene.camera->viewMatrix);
    scene.shader->setUniform("ProjectionMatrix", scene.camera->perspective);
    scene.shader->setUniform("Texture", *texture);

    mesh->render();


    for (auto& child : children) {
        child->render(scene);
    }


}