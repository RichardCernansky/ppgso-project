//
// Created by Richard Čerňanský on 12/10/2024.
//
//my files
#include "src/window.cpp"
#include <glm/fwd.hpp>
#include <random>
#include "objects/tree.h"

// Function to generate a random floating-point number in a given range [min, max]
float randomFloat(float min, float max) {
    // Create a random number generator
    static std::random_device rd;  // Seed for random number engine
    static std::mt19937 gen(rd());  // Mersenne Twister engine for generating random numbers

    // Create a uniform real distribution for floating-point numbers in the given range
    std::uniform_real_distribution<float> dist(min, max);

    // Return a random floating-point number in the range [min, max]
    return dist(gen);
}

// Function to generate random 3D position for the tree
glm::mat4 generateRandomTreeModelMatrix() {
    // Generate random position for the tree
    float x = randomFloat(-20.0f, 20.0f);
    float z = randomFloat(-20.0f, 20.0f);
    float y = 0.0f;

    // Generate random scale for the tree
    float scale = randomFloat(0.1f, 0.3f);

    // Create transformation matrix
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
    model = glm::scale(model, glm::vec3(scale, scale, scale));
    return model;
}


// function to create a rotation matrix to align forward direction to target direction
glm::mat4 rotateToFaceDirection(const glm::vec3& base_forward, const glm::vec3& targetDirection) {
    // normalize the direction to make sure it is a unit vector
    glm::vec3 normalizedTarget = glm::normalize(targetDirection);
    // calculate the rotation axis using the cross product
    glm::vec3 rotationAxis = glm::normalize(glm::cross(base_forward, normalizedTarget));
    // Calculate the rotation angle using the dot product
    float dotProduct = glm::dot(base_forward, normalizedTarget);
    float angle = acos(glm::clamp(dotProduct, -1.0f, 1.0f));  // Clamp to avoid precision issues
    // If the direction vectors are nearly parallel, no rotation is needed
    if (glm::length(rotationAxis) < 1e-6) {
        return glm::mat4(1.0f);  // Identity matrix
    }
    // create a rotation matrix that rotates the object around the rotationAxis by the calculated angle
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, rotationAxis);
    return rotationMatrix;
}

// Define the Light struct
struct Light {
    glm::vec3 position;
    float padding1;        // Padding for std140 alignment
    glm::vec3 color;
    float ambientStrength;
    float diffuseStrength;
    float specularStrength;
    float padding2;        // Padding for std140 alignment
};

const int NUM_LIGHTS = 1; // Define the number of lights

// Function to set up lights and create UBO, linking it to the shader
GLuint set_up_lights(GLuint shaderProgram) {
    // Create and initialize the UBO
    GLuint lightUBO;
    glGenBuffers(1, &lightUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, lightUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(Light) * NUM_LIGHTS, NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // Bind the UBO to binding point 0
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, lightUBO);

    // Define and initialize the light array with example data
    Light lights[NUM_LIGHTS];
    for (int i = 0; i < NUM_LIGHTS; ++i) {
        lights[i].position = glm::vec3(1.0f * i, 1.0f, 1.0f); // Example positions
        lights[i].color = glm::vec3(1.0f, 0.8f, 0.6f);         // Example colors (warm light)
        lights[i].ambientStrength = 0.2f;                     // Example ambient strength
        lights[i].diffuseStrength = 0.8f;                     // Example diffuse strength
        lights[i].specularStrength = 1.0f;                    // Example specular strength
    }

    // Upload the light data to the UBO
    glBindBuffer(GL_UNIFORM_BUFFER, lightUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Light) * NUM_LIGHTS, &lights);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // Link the UBO to the shader program (scene.shader)
    GLuint uniformBlockIndex = glGetUniformBlockIndex(shaderProgram, "LightBlock");
    if (uniformBlockIndex == GL_INVALID_INDEX) {
        std::cerr << "Error: LightBlock not found in shader program." << std::endl;
    } else {
        glUniformBlockBinding(shaderProgram, uniformBlockIndex, 0);
    }

    // Return the UBO ID for potential future updates
    return lightUBO;
}


