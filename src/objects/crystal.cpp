#include "crystal.h"
#include "src/globals.h"
// Static resources initialization
std::unique_ptr<ppgso::Mesh> Crystal::mesh;
std::unique_ptr<ppgso::Texture> Crystal::texture;

Crystal::Crystal() {
    // Load mesh if not already loaded
    if (!mesh) {
        mesh = std::make_unique<ppgso::Mesh>("crystal.obj");
    }
}

bool Crystal::update_child(float dTime, Scene &scene, glm::mat4 parentModelMatrix) {
    // Update the model matrix
    modelMatrix = glm::mat4{1.0f};
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::rotate(modelMatrix, rotation.x, glm::vec3{1, 0, 0});
    modelMatrix = glm::rotate(modelMatrix, rotation.y, glm::vec3{0, 1, 0});
    modelMatrix = glm::rotate(modelMatrix, rotation.z, glm::vec3{0, 0, 1});
    modelMatrix = glm::scale(modelMatrix, scale);
    modelMatrix = parentModelMatrix * modelMatrix;

    return true;
}

// Update method
bool Crystal::update(float dTime, Scene &scene) {
    return true;
}

void Crystal::render(Scene &scene) {
    // Calculate "breathing" effect for color intensity
    float time = (float)glfwGetTime(); // Get the global time
    float intensity = (sin(time) * 0.5f) + 0.5f; // Sinusoidal intensity in range [0, 1]

    // Base color of the crystal
    glm::vec3 baseColor = glm::vec3(0.1f, 1.0f, 0.5f); // Greenish glow
    glm::vec3 minColor = baseColor * 0.5f;             // Half-intensity dim color
    glm::vec3 breathingColor = minColor + (baseColor - minColor) * intensity;
    crystal_color = breathingColor;
    // Use the scene's shader
    // Use the shadow projection matrix
    glm::mat4 shadowMatrix = calculateShadowMatrix(moonLight_position, glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));

    // Render the shadow
    scene.colorShader->use();
    scene.colorShader->setUniform("ModelMatrix", shadowMatrix * modelMatrix);
    scene.colorShader->setUniform("ViewMatrix", scene.camera->viewMatrix);
    scene.colorShader->setUniform("ProjectionMatrix", scene.camera->perspective);

    // Render the pig's shadow as a black silhouette
    glDisable(GL_DEPTH_TEST); // Prevent z-fighting
    scene.colorShader->setUniform("Color", glm::vec3(0.0f, 0.0f, 0.0f)); // Black shadow
    mesh->render();
    glEnable(GL_DEPTH_TEST);

    // Render the pig
    scene.colorShader->use();
    scene.colorShader->setUniform("ModelMatrix", modelMatrix);
    scene.colorShader->setUniform("ViewMatrix", scene.camera->viewMatrix);
    scene.colorShader->setUniform("ProjectionMatrix", scene.camera->perspective);
    scene.colorShader->setUniform("Color", breathingColor);

    mesh->render();
}
