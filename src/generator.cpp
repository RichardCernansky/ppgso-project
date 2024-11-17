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

std::pair<glm::mat4, glm::mat4> generateRandomTreeModelMatrix() {
    // Generate random position for the tree
    float x = randomFloat(-20.0f, 20.0f);
    float z = randomFloat(-20.0f, 20.0f);
    float y = 0.0f;

    // Generate random scale for the tree
    float scale = randomFloat(0.01f, 0.014f);

    // Create transformation matrices
    glm::mat4 model_child = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
    glm::mat4 model_tree = glm::scale(model_child, glm::vec3(scale, scale, scale));

    // Return as a pair
    return std::make_pair(model_tree, model_child);
}// function to create a rotation matrix to align forward direction to target direction
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
    float padding1;          // Padding for std140 alignment
    glm::vec3 color;
    float ambientStrength;
    float diffuseStrength;
    float specularStrength;
    float padding2;          // Padding for std140 alignment
    glm::vec3 direction;     // Direction for reflector lights
    float cutoffAngle;       // Spotlight cutoff angle in degrees
    int flag;                // 0 for non-spotlight, 1 for spotlight
    float padding3;          // Padding to maintain alignment (std140)
};

// Update the number of lights to include the reflector light
const int NUM_LIGHTS = 2; // Adjust as needed for multiple lights

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

    // Define and initialize the light array to simulate moonlight and reflector light
    Light lights[NUM_LIGHTS];

    // Moonlight setup (light 0)
    lights[0].position = glm::vec3(0.0f, 50.0f, 0.0f);  // High position to simulate the moon
    lights[0].color = glm::vec3(0.6f, 0.6f, 0.8f);      // Cool light blue color
    lights[0].ambientStrength = 0.7f;
    lights[0].diffuseStrength = 0.7f;
    lights[0].specularStrength = 0.5f;
    lights[0].direction = glm::vec3(0.0f, -1.0f, 0.0f); // Not relevant for non-reflector
    lights[0].cutoffAngle = 180.0f;                     // Full spread (no cutoff)
    lights[0].flag = 0;                                 // Non-spotlight

    // Reflector light setup (light 1)
    lights[1].position = glm::vec3(0.0f, 2.0f, -1.0f); // Position for reflector light
    lights[1].color = glm::vec3(1.0f, 0.9f, 0.7f);      // Warm light color
    lights[1].ambientStrength = 10.f;
    lights[1].diffuseStrength = 100.0f;
    lights[1].specularStrength = 10.f;
    lights[1].direction = glm::vec3(0, -1.0f, -1); // Direction for reflector
    lights[1].cutoffAngle = 30.0f;                       // Spotlight angle (in degrees)
    lights[1].flag = 1;                                  // Spotlight

    // Upload the light data to the UBO
    glBindBuffer(GL_UNIFORM_BUFFER, lightUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Light) * NUM_LIGHTS, &lights);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // Link the UBO to the shader program
    GLuint uniformBlockIndex = glGetUniformBlockIndex(shaderProgram, "LightBlock");
    if (uniformBlockIndex == GL_INVALID_INDEX) {
        std::cerr << "Error: LightBlock not found in shader program." << std::endl;
    } else {
        glUniformBlockBinding(shaderProgram, uniformBlockIndex, 0);
    }

    // Return the UBO ID for potential future updates
    return lightUBO;
}



