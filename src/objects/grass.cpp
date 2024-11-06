#include <iostream>
#include <vector>
#include <random>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <ppgso/ppgso.h>
#include <shaders/instanced_glsl.h> // Include your instanced vertex shader
#include <shaders/texture_frag_glsl.h>
#include "src/scene.cpp"
#include "src/renderable.h"

const int INSTANCE_COUNT = 50;

class GrassPatch : public Renderable {
private:
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> texCoords;
    std::vector<glm::vec3> instancePositions;

    struct face {
        GLuint v0, v1, v2;
    };

    std::vector<face> mesh;
    GLuint vao, vbo, tbo, ibo, instanceVBO;
    ppgso::Shader program{instanced_glsl, texture_frag_glsl};
    ppgso::Texture texture{ppgso::image::loadBMP("grass2.bmp")};

public:
    GrassPatch() {
        generateMesh();
        generateInstances();
    }

    void generateMesh() {
        vertices = {
            { -5.0f, -5.0f, 0.0f }, // Scaled by 10
            { 5.0f, -5.0f, 0.0f },
            { 5.0f, 5.0f, 0.0f },
            { -5.0f, 5.0f, 0.0f }
        };

        texCoords = {
            { 0.0f, 0.0f },
            { 1.0f, 0.0f },
            { 1.0f, 1.0f },
            { 0.0f, 1.0f }
        };

        mesh = {
            { 0, 1, 2 },
            { 2, 3, 0 }
        };

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

        auto position_attrib = program.getAttribLocation("Position");
        glEnableVertexAttribArray(position_attrib);
        glVertexAttribPointer(position_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glGenBuffers(1, &tbo);
        glBindBuffer(GL_ARRAY_BUFFER, tbo);
        glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(glm::vec2), texCoords.data(), GL_STATIC_DRAW);

        auto texCoord_attrib = program.getAttribLocation("TexCoord");
        glEnableVertexAttribArray(texCoord_attrib);
        glVertexAttribPointer(texCoord_attrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

        glGenBuffers(1, &ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.size() * sizeof(face), mesh.data(), GL_STATIC_DRAW);
    }

    void generateInstances() {
        instancePositions.resize(INSTANCE_COUNT);
        std::default_random_engine generator;
        std::uniform_real_distribution<float> distribution(-5.0f, 5.0f);

        // Generate random positions for each instance
        for (int i = 0; i < INSTANCE_COUNT; ++i) {
            glm::vec3 randomPos{distribution(generator), 0.0f, distribution(generator)};
            instancePositions[i] = randomPos;
        }

        // Debug output to check the generated positions
        std::cout << "First instance Z-position: " << instancePositions[0].z << std::endl;

        // Upload instance data (positions) to OpenGL
        glGenBuffers(1, &instanceVBO);
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferData(GL_ARRAY_BUFFER, INSTANCE_COUNT * sizeof(glm::vec3), instancePositions.data(), GL_STATIC_DRAW);

        // Bind the VAO and set up the instance attribute
        glBindVertexArray(vao);

        // Layout location 2 for instance position attribute
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glVertexAttribDivisor(2, 1); // This makes the attribute instanced

        glBindVertexArray(0); // Unbind the VAO to prevent accidental changes
    }


    bool update(float dTime, Scene &scene) override {
        return true;
    }

    void render(Scene &scene) override {
        program.use();
        program.setUniform("ProjectionMatrix", scene.camera->perspective);
        program.setUniform("ViewMatrix", scene.camera->viewMatrix);
        program.setUniform("Texture", texture);

        glBindVertexArray(vao);
        glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(mesh.size() * 3), GL_UNSIGNED_INT, 0, INSTANCE_COUNT);
        glBindVertexArray(0);
    }
};





