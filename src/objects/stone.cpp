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

    glActiveTexture(GL_TEXTURE0);
    texture->bind();
    // Use the shadow projection matrix
    glm::mat4 shadowMatrix = calculateShadowMatrix(glm::vec3(-50.0f, 50.0f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));

    scene.shader->use();
    scene.colorShader->setUniform("ModelMatrix", shadowMatrix * modelMatrix);
    scene.colorShader->setUniform("ViewMatrix", scene.camera->viewMatrix);
    scene.colorShader->setUniform("ProjectionMatrix", scene.camera->perspective);

    // Render the pig's shadow as a black silhouette
    glDisable(GL_DEPTH_TEST); // Prevent z-fighting
    scene.colorShader->setUniform("Color", glm::vec3(0.0f, 0.0f, 0.0f)); // Black shadow
    mesh->render();
    glEnable(GL_DEPTH_TEST);

    // Render the pig
    scene.shader->use();
    scene.shader->setUniform("ModelMatrix", modelMatrix);
    scene.shader->setUniform("ViewMatrix", scene.camera->viewMatrix);
    scene.shader->setUniform("ProjectionMatrix", scene.camera->perspective);
    scene.shader->setUniform("Texture", *texture);

    mesh->render();
}
