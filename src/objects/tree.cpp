#include <ppgso/ppgso.h>
#include "../scene.cpp"
#include "../renderable.h"

class Tree final : public Renderable
{
    glm::mat4 modelMatrix{1.0f};

    // Static resources
    std::unique_ptr<ppgso::Mesh> mesh;
    std::unique_ptr<ppgso::Texture> texture;

public:
    glm::vec3 position{0, 0, -4};
    glm::vec3 scale{0.1, 0.1, 0.1};

    Tree()
    {
        if (!texture) {
            auto image = ppgso::image::loadBMP("camp_fire.bmp");
            if (image.width == 0 || image.height == 0) {
                std::cerr << "Failed to load image: scrub-ground.bmp" << std::endl;
                return;
            }
            texture = std::make_unique<ppgso::Texture>(std::move(image));
        }
        if (!mesh)
            mesh = std::make_unique<ppgso::Mesh>("tree.obj");
    }

    bool update(float dTime, Scene &scene) override
    {
        modelMatrix = glm::mat4{1.0f};
        modelMatrix = glm::translate(modelMatrix, position);
        modelMatrix = glm::rotate(modelMatrix, glm::radians(0.0f), glm::vec3{-1, 0, 0});
        modelMatrix = glm::scale(modelMatrix, scale);

        return true;
    }

    void render(Scene &scene) override
    {
        // Render the object
        scene.shader->use();
        scene.shader->setUniform("ModelMatrix", modelMatrix);
        scene.shader->setUniform("ViewMatrix", scene.camera->viewMatrix);
        scene.shader->setUniform("ProjectionMatrix", scene.camera->perspective);
        scene.shader->setUniform("Texture", *texture);

        // light
        setLightShader(scene);

        mesh->render();
    }

    void setLightShader(Scene &scene) const
    {
        //
    }
};