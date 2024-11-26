#include "crystal.h"
#include "src/globals.h"
std::unique_ptr<ppgso::Mesh> Crystal::mesh;
std::unique_ptr<ppgso::Texture> Crystal::texture;

Crystal::Crystal() {
    if (!mesh) {
        mesh = std::make_unique<ppgso::Mesh>("crystal.obj");
    }
}

bool Crystal::update_child(float dTime, Scene &scene, glm::mat4 parentModelMatrix) {
    modelMatrix = glm::mat4{1.0f};
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::rotate(modelMatrix, rotation.x, glm::vec3{1, 0, 0});
    modelMatrix = glm::rotate(modelMatrix, rotation.y, glm::vec3{0, 1, 0});
    modelMatrix = glm::rotate(modelMatrix, rotation.z, glm::vec3{0, 0, 1});
    modelMatrix = glm::scale(modelMatrix, scale);
    modelMatrix = parentModelMatrix * modelMatrix;

    return true;
}

bool Crystal::update(float dTime, Scene &scene) {
    return true;
}

void Crystal::render(Scene &scene) {
    float time = (float)glfwGetTime();
    float intensity = (sin(time*2) * 0.5f) + 0.5f;

    glm::vec3 baseColor = glm::vec3(35/255,35/255,255/255);
    glm::vec3 minColor = baseColor * 0.5f;
    glm::vec3 breathingColor = minColor + (baseColor - minColor) * intensity;
    crystal_color = breathingColor;

    glm::mat4 shadowMatrix = calculateShadowMatrix(moonLight_position, glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
    shadowMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, shadow_y_offset, 0.0f)) * shadowMatrix;

    scene.colorShader->use();
    scene.colorShader->setUniform("ModelMatrix", shadowMatrix * modelMatrix);
    scene.colorShader->setUniform("ViewMatrix", scene.camera->viewMatrix);
    scene.colorShader->setUniform("ProjectionMatrix", scene.camera->perspective);
    scene.colorShader->setUniform("Color", glm::vec3(0.0f, 0.0f, 0.0f)); // Black shadow

    mesh->render();

    scene.colorShader->use();
    scene.colorShader->setUniform("ModelMatrix", modelMatrix);
    scene.colorShader->setUniform("ViewMatrix", scene.camera->viewMatrix);
    scene.colorShader->setUniform("ProjectionMatrix", scene.camera->perspective);
    scene.colorShader->setUniform("Color", breathingColor);

    mesh->render();
}
