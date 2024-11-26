#include "horseFly.h"
#include "src/globals.h"

std::unique_ptr<ppgso::Mesh> HorseFly::mesh;
std::unique_ptr<ppgso::Texture> HorseFly::texture;

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

bool HorseFly::update(float dTime, Scene &scene) {
    return true;
}

bool HorseFly::update_child(float dTime, Scene &scene, glm::mat4 parentModelMatrix) {
    timeInState += dTime;

    if (timeInState >= 2.0f) {
        goingUp = !goingUp;
        timeInState = 0.0f;
    }

    if (goingUp) {
        position.y += 1.0f * dTime;
    } else {
        position.y -= 1.0f * dTime;
    }
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3{-1, 0, 0});
    modelMatrix = glm::scale(modelMatrix, scale);
    modelMatrix = parentModelMatrix * modelMatrix;

    return true;
}

void HorseFly::render(Scene &scene) {

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
}