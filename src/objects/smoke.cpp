//
// Created by Bruno Kristián on 21/10/2024.
//

#include "smoke.h"
// Static resources (shared among all instances of Pig)
std::unique_ptr<ppgso::Mesh> Smoke::mesh;
std::unique_ptr<ppgso::Texture> Smoke::texture;

// Constructor
Smoke::Smoke(){

    if (!texture) {
        auto image = ppgso::image::loadBMP("grey.bmp");
        if (image.width == 0 || image.height == 0) {
            std::cerr << "Failed to load texture: grey.bmp" << std::endl;
            return;
        }
        texture = std::make_unique<ppgso::Texture>(std::move(image));
    }

    if (!mesh) {
        mesh = std::make_unique<ppgso::Mesh>("smoke.obj");  // Load the boar model
        if (!mesh) {
            std::cerr << "Failed to load mesh: smoke.obj" << std::endl;
        }
    }
}

bool Smoke::update(float dTime, Scene &scene) {
    return true;
}

// Update method
bool Smoke::update_child(float dTime, Scene &scene, glm::mat4 parentModelMatrix) {

    modelMatrix = glm::mat4{1.0f};
    modelMatrix = parentModelMatrix * modelMatrix;
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3{0, 0, 1});
    modelMatrix = glm::scale(modelMatrix, scale);

    return true;
}

void Smoke::render(Scene &scene) {
    float transparency = 0.5f;

    scene.shader->use();

    scene.shader->setUniform("ModelMatrix", modelMatrix);
    scene.shader->setUniform("ViewMatrix", scene.camera->viewMatrix);
    scene.shader->setUniform("ProjectionMatrix", scene.camera->perspective);
    scene.shader->setUniform("Texture", *texture);
    scene.shader->setUniform("Transparency", transparency);

    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);

    // Render the mesh
    mesh->render();


    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}




