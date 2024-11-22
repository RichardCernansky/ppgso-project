#include <ppgso/ppgso.h>
#include "../scene.cpp"
#include "../renderable.h"
#include "src/globals.h"


class Lighthouse final : public Renderable {
    glm::mat4 modelMatrix{1.0f};

    // Static resources
    std::unique_ptr<ppgso::Mesh> mesh;
    std::unique_ptr<ppgso::Texture> texture;

public:
    // Position, rotation, and scale of the lighthouse
    glm::vec3 position{-5, 0, 5};
    glm::vec3 rotation{0, 0, 0};
    glm::vec3 scale{0.3, 0.3, 0.3};

    // Constructor
    Lighthouse() {
        if (!texture) {
            auto image = ppgso::image::loadBMP("bricks.bmp");
            if (image.width == 0 || image.height == 0) {
                std::cerr << "Failed to load image: lighthouse.bmp" << std::endl;
                return;
            }
            texture = std::make_unique<ppgso::Texture>(std::move(image));
        }
        if (!mesh)
            mesh = std::make_unique<ppgso::Mesh>("lighthouse1.obj");
    }

    // Update the lighthouse's transformation matrix
    bool update(float dTime, Scene &scene) override {
        modelMatrix = glm::mat4{1.0f};
        modelMatrix = glm::translate(modelMatrix, position);
        modelMatrix = glm::scale(modelMatrix, scale);

        return true;
    }

    // Render the lighthouse
    void render(Scene &scene) override {
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
        scene.shader->use();
        scene.shader->setUniform("ModelMatrix", modelMatrix);
        scene.shader->setUniform("ViewMatrix", scene.camera->viewMatrix);
        scene.shader->setUniform("ProjectionMatrix", scene.camera->perspective);
        scene.shader->setUniform("Texture", *texture);

        mesh->render();
    }


};
