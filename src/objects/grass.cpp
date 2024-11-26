#include <iostream>
#include <vector>

#define GLM_ENABLE_EXPERIMENTAL
#include <random>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>

#include <ppgso/ppgso.h>

#include <shaders/texture_vert_glsl.h>
#include <shaders/texture_frag_glsl.h>
#include <shaders/instanced_glsl.h>

#include "src/scene.cpp"
#include "src/renderable.h"
#define INSTANCE_COUNT 30000

class GrassPatch : public Renderable {
private:
  std::vector<glm::vec3> vertices;
  std::vector<glm::vec2> texCoords;

  struct face {
    GLuint v0, v1, v2;
  };

  std::vector<face> mesh;
  GLuint vao, vbo, tbo, ibo,instanceVBO;
  glm::mat4 modelMatrix{1.0f};
  glm::vec3 controlPoints[4][4];
  std::vector<glm::vec3> instancePositions;

  glm::vec3 bezierPoint(const glm::vec3 controlPoints[4], float t) {
    glm::vec3 p0 = glm::mix(controlPoints[0], controlPoints[1], t);
    glm::vec3 p1 = glm::mix(controlPoints[1], controlPoints[2], t);
    glm::vec3 p2 = glm::mix(controlPoints[2], controlPoints[3], t);
    glm::vec3 p3 = glm::mix(p0, p1, t);
    glm::vec3 p4 = glm::mix(p1, p2, t);
    return glm::mix(p3, p4, t);
  }

  ppgso::Shader program{instanced_glsl, texture_frag_glsl};
  ppgso::Texture texture{ppgso::image::loadBMP("grass2.bmp")};

public:
  glm::vec3 position{3, 0, -3};
  glm::vec3 scale{5, 0.1, 5};

  GrassPatch(glm::vec3 initControlPoints[4][4]) {
    // Initialize control points
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        controlPoints[i][j] = initControlPoints[i][j];
      }
    }

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &tbo);
    glGenBuffers(1, &ibo);
    generateInstances();
  }

  void generateMesh() {
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

        glm::vec3 surfacePoint = bezierPoint(curveU, v);
        vertices.push_back(surfacePoint);

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



    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

    auto position_attrib = program.getAttribLocation("Position");
    glEnableVertexAttribArray(position_attrib);
    glVertexAttribPointer(position_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, tbo);
    glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(glm::vec2), texCoords.data(), GL_STATIC_DRAW);

    auto texCoord_attrib = program.getAttribLocation("TexCoord");
    glEnableVertexAttribArray(texCoord_attrib);
    glVertexAttribPointer(texCoord_attrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.size() * sizeof(face), mesh.data(), GL_STATIC_DRAW);
  }

  void generateInstances() {
    instancePositions.resize(INSTANCE_COUNT);
    std::default_random_engine generator;

    std::uniform_real_distribution<float> distribution_x(50.0f, 70.0f);
    std::uniform_real_distribution<float> distribution_z(-20.0f, 20.0f);

    for (int i = 0; i < INSTANCE_COUNT; ++i) {
      float x = distribution_x(generator);
      float z = distribution_z(generator);
      float y = 0.0f; // Assuming ground level

      glm::vec3 randomPos{y, x, z};
      instancePositions[i] = randomPos;
    }

    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, INSTANCE_COUNT * sizeof(glm::vec3), instancePositions.data(), GL_STATIC_DRAW);

    glBindVertexArray(vao);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glVertexAttribDivisor(2, 1); // This makes the attribute instanced

    glBindVertexArray(0);
  }

  bool update(float dTime, Scene &scene) override{
    return true;
  }

  bool update_child(float dTime, Scene &scene, glm::mat4 parentMatrix) override {
    static float accumulatedTime = 0.0f;
    accumulatedTime += dTime;

    float frequency = 2.5f;
    float amplitude = 0.01f;
    float spatialFrequency = 3.0f;

    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        float wave = amplitude * sinf(frequency * accumulatedTime + spatialFrequency * j);
        controlPoints[i][j].z = wave;
      }
    }

    generateMesh();
    modelMatrix = glm::mat4{1.0f};
    modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(0, -1, 0));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(0, 0, 1));
    modelMatrix = glm::scale(modelMatrix, scale);
    modelMatrix = parentMatrix * modelMatrix;
    return true;
  }

  void render(Scene &scene) override {
    program.use();
    program.setUniform("ModelMatrix", modelMatrix);;
    program.setUniform("ProjectionMatrix", scene.camera->perspective);
    program.setUniform("ViewMatrix", scene.camera->viewMatrix);
    program.setUniform("Texture", texture);

    glBindVertexArray(vao);
    glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(mesh.size() * 3), GL_UNSIGNED_INT, 0, INSTANCE_COUNT);
    glBindVertexArray(0);
  }
};