#include "particles.h"
#include "../generator.h"
#include "src/globals.h"

std::unique_ptr<ppgso::Mesh> Particles::mesh;
std::unique_ptr<ppgso::Texture> Particles::texture;

Particles::Particles() {
    if (!texture) {
        auto image = ppgso::image::loadBMP("grey.bmp");
        if (image.width == 0 || image.height == 0) {
            std::cerr << "Failed to load texture: grey.bmp" << std::endl;
            return;
        }
        texture = std::make_unique<ppgso::Texture>(std::move(image));
    }

    if (!mesh) {
        mesh = std::make_unique<ppgso::Mesh>("particles.obj");  // Load the sphere model
        if (!mesh) {
            std::cerr << "Failed to load mesh: sphere.obj" << std::endl;
        }
    }


    position = {randomFloat(-100.0f, 100.0f), randomFloat(50.0f, 500.0f), randomFloat(400.0f, 0.0f)};
    speed = {randomFloat(0.0f, 1.0f), randomFloat(0.0f, 1.0f), randomFloat(0.0f, 1.0f)};
    scale_one = randomFloat(20.0f, 50.0f);
    scale = {scale_one, scale_one, scale_one};
    float age = 0;

}

bool Particles::update(float dTime, Scene &scene) {
    return true;
}

void Particles::reset() {
    position = {randomFloat(-100.0f, 100.0f), randomFloat(50.0f, 500.0f), randomFloat(400.0f, 0.0f)};
    speed = {randomFloat(0.0f, 1.0f), randomFloat(0.0f, 1.0f), randomFloat(0.0f, 1.0f)};
    scale_one = randomFloat(20.0f, 50.0f);
    scale = {scale_one, scale_one, scale_one};
    age = 0.0f;
    transparency = 0.5f;
}

bool Particles::isTransparent() const {
    return transparency < 1.0f;
}

glm::vec3 Particles::getGlobalPosition() const {
    return glm::vec3(modelMatrix[3].x, modelMatrix[3].y, modelMatrix[3].z);
}

float Particles::calculateDepthFromCamera(const glm::vec3& cameraPosition) const {
    glm::vec3 globalPosition = getGlobalPosition();
    return glm::length(cameraPosition - globalPosition);
}

bool Particles::update_child(float dTime, Scene &scene, glm::mat4 parentModelMatrix) {

    modelMatrix = glm::mat4{1.0f};
    modelMatrix = parentModelMatrix * modelMatrix;
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::scale(modelMatrix, scale);

    position.y += speed.y * age;
    position.x += sin(age * 2.0f);
    position.z += cos(age * 2.0f);

    transparency = 0.5f - (position.y / 2000.0f);
    if (transparency < 0.0f) transparency = 0.0f;

    if (position.y > 1000.0f) {
        reset();
    }

    age += dTime;
    return true;
}

void Particles::render(Scene &scene) {
    scene.shader->use();
    scene.shader->setUniform("ModelMatrix", modelMatrix);
    scene.shader->setUniform("ViewMatrix", scene.camera->viewMatrix);
    scene.shader->setUniform("ProjectionMatrix", scene.camera->perspective);
    scene.shader->setUniform("Texture", *texture);


    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);

    mesh->render();

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}
