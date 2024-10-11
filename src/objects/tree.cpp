#include "tree.h"

// Define the static members
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
        mesh = std::make_unique<ppgso::Mesh>("tree.obj");
        if (!mesh) {
            std::cerr << "Failed to load mesh: tree.obj" << std::endl;
        }
    }
}

bool Tree::update(float dTime, Scene &scene) {
    modelMatrix = glm::mat4{1.0f};
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(0.0f), glm::vec3{-1, 0, 0});
    modelMatrix = glm::scale(modelMatrix, scale);
    return true;
}

// Implement the correct render method
void Tree::render(Scene &scene) {
    scene.shader->use();
    scene.shader->setUniform("ModelMatrix", modelMatrix);
    scene.shader->setUniform("ViewMatrix", scene.camera->viewMatrix);
    scene.shader->setUniform("ProjectionMatrix", scene.camera->perspective);
    scene.shader->setUniform("Texture", *texture);
    mesh->render();
}

//bruno pridal
// Optional instanced rendering method
void Tree::renderInstanced(Scene &scene, const std::vector<glm::mat4> &instanceTransforms) {
    scene.shader->use();
    scene.shader->setUniform("ViewMatrix", scene.camera->viewMatrix);
    scene.shader->setUniform("ProjectionMatrix", scene.camera->perspective);
    scene.shader->setUniform("Texture", *texture);

    setLightShader(scene);

    // Render each instance with its own transform
    for (const auto& transform : instanceTransforms) {
        scene.shader->setUniform("ModelMatrix", transform);
        mesh->render();
    }
}

bool Tree::update_child(float d, Scene &scene, glm::mat4 ParentModelMatrix) {
    return false;
}

void Tree::setLightShader(Scene &scene) const {
    // Set up lighting here if needed
}
