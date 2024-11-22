#include "MySphere.h"

#include <shaders/color_frag_glsl.h>
#include <shaders/color_vert_glsl.h>
#include <shaders/phong_vert_glsl.h>
#include <shaders/point_frag_glsl.h>


// Define static members
std::unique_ptr<ppgso::Mesh> MySphere::mesh;
std::unique_ptr<ppgso::Texture> MySphere::texture;

// Constructor
MySphere::MySphere() {

    if (!texture) {
        auto image = ppgso::image::loadBMP("grey.bmp");
        if (image.width == 0 || image.height == 0) {
            std::cerr << "Failed to load texture: grey.bmp" << std::endl;
            return;
        }
        texture = std::make_unique<ppgso::Texture>(std::move(image));
    }

    // Load mesh if it hasn't been loaded yet
    if (!mesh)
        mesh = std::make_unique<ppgso::Mesh>("sphere.obj");
}

// Update the sphere's transformation matrix
bool MySphere::update(float dTime, Scene &scene) {
    // Reset the model matrix
    modelMatrix = glm::mat4{1.0f};

    // Apply transformations
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::rotate(modelMatrix, rotation.x, glm::vec3(1, 0, 0));
    modelMatrix = glm::rotate(modelMatrix, rotation.y, glm::vec3(0, 1, 0));
    modelMatrix = glm::rotate(modelMatrix, rotation.z, glm::vec3(0, 0, 1));
    modelMatrix = glm::scale(modelMatrix, scale);

    return true;
}

// Render the sphere
void MySphere::render(Scene &scene) {

    // Use the scene's shader
    scene.colorShader->use();
    // shader->setUniform("OverallColor", glm::vec3(1.0f, 1.0f, 1.0f));
    scene.colorShader->setUniform("ModelMatrix", modelMatrix);
    scene.colorShader->setUniform("ViewMatrix", scene.camera->viewMatrix);
    scene.colorShader->setUniform("ProjectionMatrix", scene.camera->perspective);
    scene.colorShader->setUniform("Color",glm::vec3(1.0f, 1.0f, 1.0f));

    // Bind the texture
    scene.colorShader->setUniform("Texture", *texture);

    // Render the mesh
    mesh->render();
}
