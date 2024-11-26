#include "gas.h"
std::unique_ptr<ppgso::Mesh> Gas::mesh;
std::unique_ptr<ppgso::Texture> Gas::texture;

Gas::Gas(){

    if (!texture) {
        auto image = ppgso::image::loadBMP("green.bmp");
        if (image.width == 0 || image.height == 0) {
            std::cerr << "Failed to load texture: boar_texture.bmp" << std::endl;
            return;
        }
        texture = std::make_unique<ppgso::Texture>(std::move(image));
    }

    if (!mesh) {
        mesh = std::make_unique<ppgso::Mesh>("gas.obj");
        if (!mesh) {
            std::cerr << "Failed to load mesh: gas.obj" << std::endl;
        }
    }
}

bool Gas::isTransparent() const {
    return transparency < 1.0f;
}

glm::vec3 Gas::getGlobalPosition() const {
    return glm::vec3(modelMatrix[3].x, modelMatrix[3].y, modelMatrix[3].z);
}

float Gas::calculateDepthFromCamera(const glm::vec3& cameraPosition) const {
    glm::vec3 globalPosition = getGlobalPosition();
    return glm::length(cameraPosition - globalPosition);
}

bool Gas::update(float dTime, Scene &scene) {
    return true;
}

bool Gas::update_child(float dTime, Scene &scene, glm::mat4 parentModelMatrix) {
    modelMatrix = glm::mat4(1.0f);

    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = parentModelMatrix * modelMatrix;
    for (auto& child : children) {
        child->update_child(dTime,scene, modelMatrix);
    }
    modelMatrix = glm::scale(modelMatrix, scale);

    return true;
}

void Gas::render(Scene &scene) {
    scene.shader->use();
    scene.shader->setUniform("ModelMatrix", modelMatrix);
    scene.shader->setUniform("ViewMatrix", scene.camera->viewMatrix);
    scene.shader->setUniform("ProjectionMatrix", scene.camera->perspective);
    scene.shader->setUniform("Texture", *texture);
    scene.shader->setUniform("Transparency", transparency);

    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);

    mesh->render();

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);

    for (auto& child : children) {
        child->render(scene);
    }
}




