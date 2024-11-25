//
// Created by Bruno Kristián on 18/10/2024.
//

#include "AppleTree.h"
#include "src/globals.h"


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

bool AppleTree::update(float dTime, Scene &scene) {
    return true;
}


// Update method
bool AppleTree::update_child(float dTime, Scene &scene, glm::mat4 ParentModelMatrix) {

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

    glm::mat4 shadowMatrix = calculateShadowMatrix(moonLight_position, glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));

    // Move the shadow 0.1 up on the Y-axis
    shadowMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.1f, 0.0f)) * shadowMatrix;

    // Render the shadow
    scene.colorShader->use();
    scene.colorShader->setUniform("ModelMatrix", shadowMatrix * modelMatrix);
    scene.colorShader->setUniform("ViewMatrix", scene.camera->viewMatrix);
    scene.colorShader->setUniform("ProjectionMatrix", scene.camera->perspective);

    // Render the pig's shadow as a black silhouette
    scene.colorShader->setUniform("Color", glm::vec3(0.0f, 0.0f, 0.0f)); // Black shadow
    mesh->render();

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