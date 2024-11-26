#include "stone.h"
#include "src/globals.h"

std::unique_ptr<ppgso::Mesh> Stone::mesh;
std::unique_ptr<ppgso::Texture> Stone::texture;

Stone::Stone() {
    if (!texture) {
        auto image = ppgso::image::loadBMP("rock.bmp");
        if (image.width == 0 || image.height == 0) {
            std::cerr << "Failed to load texture: rock.bmp" << std::endl;
            return;
        }
        texture = std::make_unique<ppgso::Texture>(std::move(image));
    }

    if (!mesh) {
        mesh = std::make_unique<ppgso::Mesh>("rock.obj");
        if (!mesh) {
            std::cerr << "Failed to load mesh: rock" << std::endl;
        }
    }
}

bool Stone::update(float dTime, Scene &scene) {

    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(0.0f), glm::vec3{-1, 0, 0});
    modelMatrix = glm::scale(modelMatrix, scale);


    return true;
}

bool Stone::update_child(float dTime, Scene &scene, glm::mat4 parentModelMatrix) {

    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    for (auto& child : children) {
        child->update_child(dTime, scene, modelMatrix);
    }
    modelMatrix = glm::rotate(modelMatrix, glm::radians(0.0f), glm::vec3{-1, 0, 0});
    modelMatrix = glm::scale(modelMatrix, scale);
    modelMatrix = parentModelMatrix * modelMatrix;

    return true;
}

void Stone::render(Scene &scene) {


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
