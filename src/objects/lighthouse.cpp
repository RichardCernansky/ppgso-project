#include <ppgso/ppgso.h>
#include "../scene.cpp"
#include "../renderable.h"
#include "src/globals.h"


class Lighthouse final : public Renderable {
    glm::mat4 modelMatrix{1.0f};
    float sideFactor_dir = -1;
    float sideFactor_col = 1;
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

    bool update(float dTime, Scene &scene) override{
        return true;
    }

    // Update the lighthouse's transformation matrix
    bool update_child(float dTime, Scene &scene, glm::mat4 parentMatrix) override {
        float speed_dir = 4;
        float speed_col = 1;
        if (reflector_direction.x < 0 || reflector_direction.x > 11) {
            if (reflector_direction.x < 0) {
                reflector_direction.x = 0.01;
            }else {
                reflector_direction.x = 10.9;
            }
            sideFactor_dir *= -1;
        }
        if (reflector_color.x < 0.3 || reflector_color.x > 0.9) {
            if (reflector_color.x < 0.3) {
                reflector_color.x = 0.31;
            }else {
                reflector_color.x = 0.9;
            }
            sideFactor_col *= -1;
        }

        reflector_direction = glm::vec3(reflector_direction.x + sideFactor_dir*speed_dir*dTime, reflector_direction.y, reflector_direction.z);
        reflector_color = glm::vec3(reflector_color.x + sideFactor_col*speed_col*dTime, reflector_color.y, reflector_color.z);

        modelMatrix = glm::mat4{1.0f};
        modelMatrix = glm::translate(modelMatrix, position);
        modelMatrix = glm::scale(modelMatrix, scale);
        modelMatrix = parentMatrix * modelMatrix;

        return true;
    }

    // Render the lighthouse
    void render(Scene &scene) override {
        // Use the shadow projection matrix
        glm::mat4 shadowMatrix = calculateShadowMatrix(moonLight_position, glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));

        // Move the shadow 0.1 up on the Y-axis
        shadowMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.1f, 0.0f)) * shadowMatrix;

        // Render the shadow
        scene.colorShader->use();
        scene.colorShader->setUniform("ModelMatrix", shadowMatrix * modelMatrix);
        scene.colorShader->setUniform("ViewMatrix", scene.camera->viewMatrix);
        scene.colorShader->setUniform("ProjectionMatrix", scene.camera->perspective);

        // Render the pig's shadow as a black silhouette
        scene.colorShader->setUniform("Color", glm::vec3(0.0f, 0.0f, 0.0f)); // Black shadow
        mesh->render();

        // Use the shader and set the transformation matrices
        scene.shader->use();
        scene.shader->setUniform("ModelMatrix", modelMatrix);
        scene.shader->setUniform("ViewMatrix", scene.camera->viewMatrix);
        scene.shader->setUniform("ProjectionMatrix", scene.camera->perspective);
        scene.shader->setUniform("Texture", *texture);

        // Render the mesh
        mesh->render();
    }


};
