#include "gas.h"
// Static resources (shared among all instances of Pig)
std::unique_ptr<ppgso::Mesh> Gas::mesh;
std::unique_ptr<ppgso::Texture> Gas::texture;

// Constructor
Gas::Gas(){

    if (!texture) {
        auto image = ppgso::image::loadBMP("green.bmp");  // Assuming a texture for boar
        if (image.width == 0 || image.height == 0) {
            std::cerr << "Failed to load texture: boar_texture.bmp" << std::endl;
            return;
        }
        texture = std::make_unique<ppgso::Texture>(std::move(image));
    }

    if (!mesh) {
        mesh = std::make_unique<ppgso::Mesh>("gas.obj");  // Load the boar model
        if (!mesh) {
            std::cerr << "Failed to load mesh: gas.obj" << std::endl;
        }
    }
}

// Update method
bool Gas::update(float dTime, Scene &scene) {
    return true;
}

// Update method
bool Gas::update_child(float dTime, Scene &scene, glm::mat4 parentModelMatrix) {
    // Initialize the model matrix to identity matrix
    modelMatrix = glm::mat4(1.0f);  // Identity matrix to reset transformations

    // Apply the child's local transformations (relative to its parent)
    modelMatrix = glm::translate(modelMatrix, position);  // Apply Gas's local position
    modelMatrix = parentModelMatrix * modelMatrix;
    for (auto& child : children) {
        child->update_child(dTime,scene, modelMatrix);
    }
    modelMatrix = glm::scale(modelMatrix, scale);         // Apply Gas's local scale

    return true;
}

void Gas::render(Scene &scene) {
    float transparency = 0.05f; // Adjust as needed

    scene.shader->use();

    // Set shader uniforms
    scene.shader->setUniform("ModelMatrix", modelMatrix);
    scene.shader->setUniform("ViewMatrix", scene.camera->viewMatrix);
    scene.shader->setUniform("ProjectionMatrix", scene.camera->perspective);
    scene.shader->setUniform("Texture", *texture);
    scene.shader->setUniform("Transparency", transparency);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);

    // Render the mesh
    mesh->render();

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}




