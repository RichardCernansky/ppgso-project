#include "tree.h"

std::unique_ptr<ppgso::Mesh> Tree::mesh;
std::unique_ptr<ppgso::Texture> Tree::texture;

Tree::Tree() {
    if (!texture) {
        auto image = ppgso::image::loadBMP("camp_fire.bmp");
        if (image.width == 0 || image.height == 0) {
            std::cerr << "Failed to load image: camp_fire.bmp" << std::endl;
            return;
        }
        texture = std::make_unique<ppgso::Texture>(std::move(image));
    }
    if (!mesh) {
        mesh = std::make_unique<ppgso::Mesh>("pine_tree.obj");
        if (!mesh) {
            std::cerr << "Failed to load mesh: tree2.obj" << std::endl;
        }
    }
}

bool Tree::update(float dTime, Scene &scene) {
    return true;
}

bool Tree::update_child(float dTime, Scene &scene, glm::mat4 ParentModelMatrix) {
    for (auto& child : children) {
        child->update_child(dTime,scene, childModelMatrix);
    }
    return true;
}

void Tree::render(Scene &scene) {
    scene.shader->use();
    scene.shader->setUniform("ViewMatrix", scene.camera->viewMatrix);
    scene.shader->setUniform("ProjectionMatrix", scene.camera->perspective);
    scene.shader->setUniform("Texture", *texture);
    scene.shader->setUniform("ModelMatrix", modelMatrix);
    mesh->render();

    for (auto& child : children) {
        child->render(scene);
    }
}
