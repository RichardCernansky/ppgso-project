#include <iostream>
#include <vector>
#include <random>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <ppgso/ppgso.h>
#include <shaders/texture_vert_glsl.h>
#include <shaders/texture_frag_glsl.h>
#include <shaders/instanced_glsl.h>
#include "src/scene.cpp"
#include "src/renderable.h"

const int INSTANCE_COUNT = 50;

class GrassPatch : public Renderable {
private:
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> texCoords;
    std::vector<glm::mat4> instanceMatrices; // Transformation matrices for each instance

    struct face {
        GLuint v0, v1, v2;
    };

    std::vector<face> mesh;
    GLuint vao, vbo, tbo, ibo, instanceVBO; // Add instanceVBO for instancing
    glm::mat4 modelMatrix{1.0f};
    glm::vec3 controlPoints[4][4];

    ppgso::Shader program{texture_vert_glsl, texture_frag_glsl};
    ppgso::Texture texture{ppgso::image::loadBMP("grass2.bmp")};

    glm::vec3 bezierPoint(const glm::vec3 controlPoints[4], float t) {
        glm::vec3 p0 = glm::mix(controlPoints[0], controlPoints[1], t);
        glm::vec3 p1 = glm::mix(controlPoints[1], controlPoints[2], t);
        glm::vec3 p2 = glm::mix(controlPoints[2], controlPoints[3], t);
        glm::vec3 p3 = glm::mix(p0, p1, t);
        glm::vec3 p4 = glm::mix(p1, p2, t);
        return glm::mix(p3, p4, t);
    }

public:
    GrassPatch(glm::vec3 initControlPoints[4][4]) {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                controlPoints[i][j] = initControlPoints[i][j];
            }
        }
        generateInstances();
        generateMesh();
    }

    void generateMesh() {
        std::cout << "Generating mesh..." << std::endl;
        vertices.clear();
        texCoords.clear();
        mesh.clear();
        unsigned int PATCH_SIZE = 10;
        
        for (unsigned int i = 0; i < PATCH_SIZE; i++) {
            float u = (float)i / (PATCH_SIZE - 1);
            for (unsigned int j = 0; j < PATCH_SIZE; j++) {
                float v = (float)j / (PATCH_SIZE - 1);
                glm::vec3 curveU[4];
                for (int k = 0; k < 4; k++) {
                    curveU[k] = bezierPoint(controlPoints[k], u);
                }
                vertices.push_back(bezierPoint(curveU, v));
                texCoords.push_back({u, v});
            }
        }
        for (unsigned int i = 0; i < PATCH_SIZE - 1; i++) {
            for (unsigned int j = 0; j < PATCH_SIZE - 1; j++) {
                GLuint topLeft = i * PATCH_SIZE + j;
                GLuint topRight = topLeft + 1;
                GLuint bottomLeft = (i + 1) * PATCH_SIZE + j;
                GLuint bottomRight = bottomLeft + 1;
                mesh.push_back({topLeft, bottomLeft, topRight});
                mesh.push_back({topRight, bottomLeft, bottomRight});
            }
        }

        // Copy data to OpenGL buffers
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
        instanceMatrices.resize(INSTANCE_COUNT);
        std::default_random_engine generator;
        std::uniform_real_distribution<float> distribution(-5.0f, 5.0f);

        for (int i = 0; i < INSTANCE_COUNT; ++i) {
            glm::vec3 randomPos{distribution(generator), 0.0f, distribution(generator)};
            glm::vec3 randomScale{1.0f};  // Use a uniform scale or add variation
            instanceMatrices[i] = glm::translate(glm::mat4(1.0f), randomPos) *
                                  glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, -1, 0)) *
                                  glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 0, 1)) *
                                  glm::scale(glm::mat4(1.0f), randomScale);
        }

        // Generate the instance VBO and set attributes
        glGenBuffers(1, &instanceVBO);
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferData(GL_ARRAY_BUFFER, instanceMatrices.size() * sizeof(glm::mat4), &instanceMatrices[0], GL_STATIC_DRAW);

        glBindVertexArray(vao);
        for (int i = 0; i < 4; i++) {
            glEnableVertexAttribArray(3 + i);
            glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4) * i));
            glVertexAttribDivisor(3 + i, 1);  // Update per instance
        }
    }

    bool update(float dTime, Scene &scene) override  {
        static float accumulatedTime = 0.0f;
        accumulatedTime += dTime;

        float frequency = .5f;
        float amplitude = 0.01f;
        float spatialFrequency = 3.0f;

        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                float wave = amplitude * sinf(frequency * accumulatedTime + spatialFrequency * j);
                controlPoints[i][j].z = wave;
            }
        }

        // Regenerate the mesh each frame with updated control points
        generateMesh();
        return true;
    }

    void render(Scene &scene) override {
        program.use();
        program.setUniform("ViewMatrix", scene.camera->viewMatrix);
        program.setUniform("ProjectionMatrix", scene.camera->perspective);
        program.setUniform("Texture", texture);

        glBindVertexArray(vao);
        glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(mesh.size() * 3), GL_UNSIGNED_INT, 0, INSTANCE_COUNT);
        glBindVertexArray(0);
    }
};
