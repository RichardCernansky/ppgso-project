#include "fire.h"
std::unique_ptr<ppgso::Mesh> Fire::mesh;
std::unique_ptr<ppgso::Texture> Fire::texture;

Fire::Fire(){

    if (!texture) {
        auto image = ppgso::image::loadBMP("camp_fire.bmp");
        if (image.width == 0 || image.height == 0) {
            std::cerr << "Failed to load texture: camp_fire.bmp" << std::endl;
            return;
        }
        texture = std::make_unique<ppgso::Texture>(std::move(image));
    }

    if (!mesh) {
        mesh = std::make_unique<ppgso::Mesh>("camp_fire.obj");
        if (!mesh) {
            std::cerr << "Failed to load mesh: camp_fire.obj" << std::endl;
        }
    }
}

bool Fire::update(float dTime, Scene &scene) {};

bool Fire::update_child(float dTime, Scene &scene, glm::mat4 ParentModelMatrix) {

    modelMatrix = glm::mat4{1.0f};
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::scale(modelMatrix, scale);

    for (auto& child : children) {
        child->update_child(dTime,scene, modelMatrix);
    }

    return true;
}

void Fire::render(Scene &scene) {
    scene.shader->use();
    scene.shader->setUniform("ModelMatrix", modelMatrix);
    scene.shader->setUniform("ViewMatrix", scene.camera->viewMatrix);
    scene.shader->setUniform("ProjectionMatrix", scene.camera->perspective);
    scene.shader->setUniform("Texture", *texture);

    mesh->render();

    for (auto& child : children) {
        child->render(scene);
    }
}



