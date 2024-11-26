#include "ground.h"
#include <iostream> // For std::cerr

Ground::Ground() {
    if (!texture) {
        auto image = ppgso::image::loadBMP("ground.bmp");
        if (image.width == 0 || image.height == 0) {
            std::cerr << "Failed to load image: ground.bmp" << std::endl;
            return;
        }
        texture = std::make_unique<ppgso::Texture>(std::move(image));
    }

    if (!mesh) {
        mesh = std::make_unique<ppgso::Mesh>("quad.obj");
    }
}

bool Ground::update(float dTime, Scene &scene) {
    modelMatrix = glm::mat4{1.0f};

    for (auto &child : children) {
        child->update_child(dTime, scene, modelMatrix);
    }

    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3{-1.0f, 0.0f, 0.0f});
    modelMatrix = glm::scale(modelMatrix, scale);

    return true;
}

void Ground::render(Scene &scene) {
    scene.shader->use();
    scene.shader->setUniform("ModelMatrix", modelMatrix);
    scene.shader->setUniform("ViewMatrix", scene.camera->viewMatrix);
    scene.shader->setUniform("ProjectionMatrix", scene.camera->perspective);
    scene.shader->setUniform("Texture", *texture);

    mesh->render();

    for (auto &child : children) {
        child->render(scene);
    }
}
