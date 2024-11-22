#include "horseFly.h"

// Static resources (shared among all instances of Pig)
std::unique_ptr<ppgso::Mesh> HorseFly::mesh;
std::unique_ptr<ppgso::Texture> HorseFly::texture;

// Constructor
HorseFly::HorseFly() {
    if (!texture) {
        auto image = ppgso::image::loadBMP("bricks.bmp");  // Assuming a texture for boar
        if (image.width == 0 || image.height == 0) {
            std::cerr << "Failed to load texture: boar_texture.bmp" << std::endl;
            return;
        }
        texture = std::make_unique<ppgso::Texture>(std::move(image));
    }

    if (!mesh) {
        mesh = std::make_unique<ppgso::Mesh>("horseFly.obj");  // Load the boar model
        if (!mesh) {
            std::cerr << "Failed to load mesh: gas.obj" << std::endl;
        }
    }
}

// Update method
bool HorseFly::update(float dTime, Scene &scene) {
    return true;
}

// Update method
bool HorseFly::update_child(float dTime, Scene &scene, glm::mat4 parentModelMatrix) {
    timeInState += dTime;

    if (timeInState >= 2.0f) {
        goingUp = !goingUp;    // Toggle the direction
        timeInState = 0.0f;    // Reset the timer
    }

    if (goingUp) {
        position.y += 1.0f * dTime;  // Move up at a speed of 1 unit per second
    } else {
        position.y -= 1.0f * dTime;  // Move down at a speed of 1 unit per second
    }
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3{-1, 0, 0});
    modelMatrix = glm::scale(modelMatrix, scale);
    // combine the parent's transformation matrix with the child's local transformation
    modelMatrix = parentModelMatrix * modelMatrix;


    return true;
}

void HorseFly::render(Scene &scene) {

    // Use the shadow projection matrix
    glm::mat4 shadowMatrix = calculateShadowMatrix(glm::vec3(-50.0f, 50.0f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));

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
    scene.shader->use();
    scene.shader->setUniform("ModelMatrix", modelMatrix);
    scene.shader->setUniform("ViewMatrix", scene.camera->viewMatrix);
    scene.shader->setUniform("ProjectionMatrix", scene.camera->perspective);
    scene.shader->setUniform("Texture", *texture);

    mesh->render();
}