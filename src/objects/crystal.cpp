#include "crystal.h"

// Static resources initialization
std::unique_ptr<ppgso::Mesh> Crystal::mesh;
std::unique_ptr<ppgso::Texture> Crystal::texture;

Crystal::Crystal() {


    // Load mesh if not already loaded
    if (!mesh) {
        mesh = std::make_unique<ppgso::Mesh>("crystal.obj");
    }
}

bool Crystal::update(float dTime, Scene &scene) {
    // Update the model matrix
    modelMatrix = glm::mat4{1.0f};
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::rotate(modelMatrix, rotation.x, glm::vec3{1, 0, 0});
    modelMatrix = glm::rotate(modelMatrix, rotation.y, glm::vec3{0, 1, 0});
    modelMatrix = glm::rotate(modelMatrix, rotation.z, glm::vec3{0, 0, 1});
    modelMatrix = glm::scale(modelMatrix, scale);

    return true;
}

void Crystal::render(Scene &scene) {
    // Use the scene's shader
    scene.colorShader->use();
    scene.colorShader->setUniform("ModelMatrix", modelMatrix);
    scene.colorShader->setUniform("ViewMatrix", scene.camera->viewMatrix);
    scene.colorShader->setUniform("ProjectionMatrix", scene.camera->perspective);
    scene.colorShader->setUniform("Color",glm::vec3(0.1f, 1.0f, 0.5f));

    // Render the mesh
    mesh->render();
}

void Crystal::setLightShader(Scene &scene) const {

}
