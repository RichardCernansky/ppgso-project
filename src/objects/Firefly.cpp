#include "Firefly.h"
#include <cmath>
#include <cstdlib>
#include <ctime>

Firefly::Firefly(int count)
    : fireflyCount(count) {
    srand(static_cast<unsigned int>(time(nullptr)));

    // Generate initial random positions for fireflies
    for (int i = 0; i < fireflyCount; ++i) {
        float x = ((rand() % 100) / 50.0f) - 1.0f; // Random x between -1 and 1
        float y = ((rand() % 100) / 50.0f) - 1.0f; // Random y between -1 and 1
        float z = ((rand() % 100) / 50.0f) - 1.0f; // Random z between -1 and 1
        glm::vec3 pos(x, y, z);
        initialPositions.push_back(pos);
        positions.push_back(pos);
    }

    // Create VBO for instance positions
    glGenBuffers(1, &modelMatVBO);
    glBindBuffer(GL_ARRAY_BUFFER, modelMatVBO);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3), positions.data(), GL_DYNAMIC_DRAW);

    // Set instance attributes in each buffer's VAO
    for (const auto& buffer : fireflyMesh.buffers) {
        glBindVertexArray(buffer.vao);

        // Set instance attribute for positions
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glVertexAttribDivisor(1, 1); // Update position per instance

        glBindVertexArray(0);
    }
}

bool Firefly::update(float dTime, Scene &scene) {
    // Oscillation parameters
    float oscillationSpeed = 2.0f;
    float amplitude = 0.1f;

    modelMatrices.clear(); // Clear model matrices to update for each frame

    for (int i = 0; i < fireflyCount; ++i) {
        // Compute oscillating y-position for each firefly
        positions[i].y = initialPositions[i].y + sin(dTime * oscillationSpeed + i) * amplitude;

        // Compute model matrix with translation for each instance
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, positions[i]);

        // Push updated model matrix for each instance
        modelMatrices.push_back(modelMatrix);
    }

    // Update instance VBO with new model matrices for each instance
    glBindBuffer(GL_ARRAY_BUFFER, modelMatVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, modelMatrices.size() * sizeof(glm::mat4), modelMatrices.data());

    return true;
}

void Firefly::render(Scene &scene) {
    // Use the shader from the scene
    scene.shader->use();

    // Set transformation matrices for instanced rendering
    scene.shader->setUniform("ViewMatrix", scene.camera->viewMatrix);
    scene.shader->setUniform("ProjectionMatrix", scene.camera->perspective);

    // Iterate over all buffers in fireflyMesh and render each with instancing
    for (const auto& buffer : fireflyMesh.buffers) {
        // Bind VAO for the current shape
        glBindVertexArray(buffer.vao);

        // Bind modelMatrixVBO for each shape
        glBindBuffer(GL_ARRAY_BUFFER, modelMatVBO);

        // Set model matrix attribute as a mat4 divided into four vec4s (OpenGL limitation)
        for (int i = 0; i < 4; i++) {
            glEnableVertexAttribArray(2 + i);
            glVertexAttribPointer(2 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4) * i));
            glVertexAttribDivisor(2 + i, 1);
        }

        // Render the shape using instancing
        glDrawElementsInstanced(GL_TRIANGLES, buffer.size, GL_UNSIGNED_INT, nullptr, fireflyCount);

        // Unbind VAO after rendering each shape
        glBindVertexArray(0);
    }
}

