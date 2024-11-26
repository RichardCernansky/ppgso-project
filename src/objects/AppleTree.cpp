#include "AppleTree.h"
#include "src/globals.h"


std::unique_ptr<ppgso::Mesh> AppleTree::mesh;
std::unique_ptr<ppgso::Texture> AppleTree::texture;

AppleTree::AppleTree() {
    if (!texture) {
        auto image = ppgso::image::loadBMP("apple_tree.bmp");
        if (image.width == 0 || image.height == 0) {
            std::cerr << "Failed to load texture: apple_tree_texture.bmp" << std::endl;
            return;
        }
        texture = std::make_unique<ppgso::Texture>(std::move(image));
    }

    if (!mesh) {
        mesh = std::make_unique<ppgso::Mesh>("apple_tree.obj");
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
    shadowMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, shadow_y_offset, 0.0f)) * shadowMatrix;

    scene.colorShader->use();
    scene.colorShader->setUniform("ModelMatrix", shadowMatrix * modelMatrix);
    scene.colorShader->setUniform("ViewMatrix", scene.camera->viewMatrix);
    scene.colorShader->setUniform("ProjectionMatrix", scene.camera->perspective);
    scene.colorShader->setUniform("Color", glm::vec3(0.0f, 0.0f, 0.0f));

    mesh->render();

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