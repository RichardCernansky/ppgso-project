#include "MySphere.h"

#include <shaders/color_frag_glsl.h>
#include <shaders/color_vert_glsl.h>
#include <shaders/phong_vert_glsl.h>
#include <shaders/point_frag_glsl.h>

#include "src/globals.h"

std::unique_ptr<ppgso::Mesh> MySphere::mesh;
std::unique_ptr<ppgso::Texture> MySphere::texture;

MySphere::MySphere() {

    if (!texture) {
        auto image = ppgso::image::loadBMP("grey.bmp");
        if (image.width == 0 || image.height == 0) {
            std::cerr << "Failed to load texture: grey.bmp" << std::endl;
            return;
        }
        texture = std::make_unique<ppgso::Texture>(std::move(image));
    }

    if (!mesh)
        mesh = std::make_unique<ppgso::Mesh>("sphere.obj");
}

bool MySphere::update(float dTime, Scene &scene) {
    float speed = 1;
    position.y += speed * dTime;
    position.z += speed * dTime;
    moonLight_position = position;
    modelMatrix = glm::mat4{1.0f};

    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::rotate(modelMatrix, rotation.x, glm::vec3(1, 0, 0));
    modelMatrix = glm::rotate(modelMatrix, rotation.y, glm::vec3(0, 1, 0));
    modelMatrix = glm::rotate(modelMatrix, rotation.z, glm::vec3(0, 0, 1));
    modelMatrix = glm::scale(modelMatrix, scale);

    return true;
}

void MySphere::render(Scene &scene) {
    scene.colorShader->use();
    scene.colorShader->setUniform("ModelMatrix", modelMatrix);
    scene.colorShader->setUniform("ViewMatrix", scene.camera->viewMatrix);
    scene.colorShader->setUniform("ProjectionMatrix", scene.camera->perspective);
    scene.colorShader->setUniform("Color",glm::vec3(1.0f, 1.0f, 1.0f));

    mesh->render();
}
