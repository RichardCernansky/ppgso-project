//
// Created by Richard Čerňanský on 12/10/2024.
//
//my files
#include "src/window.cpp"
#include <glm/fwd.hpp>
#include <random>
#include "objects/tree.h"
#include "globals.h"

// Function to generate a random float between min and max
float randomFloat(float min, float max) {
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}

// Generate a random tree position within a circular ring (radius between 30 and 40)
std::pair<glm::mat4, glm::mat4> generateRandomTreeModelMatrix() {
    // Generate a random radius within the range 30 to 40
    float radius = randomFloat(15.0f, 20.0f);

    // Generate a random angle in radians
    float angle = randomFloat(0.0f, 2.0f * M_PI);

    // Compute random position within the circle
    float x = radius * cos(angle);
    float z = radius * sin(angle);
    float y = 0.0f; // Assuming ground level

    // Generate a random scale for the tree
    float scale = randomFloat(0.01f, 0.014f);

    // Create transformation matrices
    glm::mat4 model_child = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
    glm::mat4 model_tree = glm::scale(model_child, glm::vec3(scale, scale, scale));

    // Return as a pair
    return std::make_pair(model_tree, model_child);
}

std::pair<glm::mat4, glm::mat4> generateRandomTreeModelMatrixAlternative() {
    // Center of the circle
    glm::vec3 circleCenter = glm::vec3(-25.0f, 0.0f, -17.0f);

    // Generate a random radius within the range 10 to 15
    float radius = randomFloat(10.0f, 15.0f);

    // Generate a random angle in radians
    float angle = randomFloat(0.0f, 2.0f * M_PI);

    // Compute random position within the circle
    float x = circleCenter.x + radius * cos(angle);
    float z = circleCenter.z + radius * sin(angle);
    float y = circleCenter.y; // Assuming ground level

    // Generate a random scale for the tree
    float scale = randomFloat(0.01f, 0.014f);

    // Create transformation matrices
    glm::mat4 model_child = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
    glm::mat4 model_tree = glm::scale(model_child, glm::vec3(scale, scale, scale));

    // Return as a pair
    return std::make_pair(model_tree, model_child);
}



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

struct Light {
    alignas(16) glm::vec3 position;  // vec3 requires 16-byte alignment, 12 bytes + 4 padding
    alignas(4) float ambientStrength;

    alignas(16) glm::vec3 color;     // vec3 requires 16-byte alignment
    alignas(4) float diffuseStrength;

    alignas(16) glm::vec3 direction; // vec3 requires 16-byte alignment
    alignas(4) float specularStrength;

    alignas(4) float cutoffAngle;    // Single float for cutoff
    alignas(4) int flag;             // Integer for light type
    alignas(8) int padding[2];       // Padding to align the struct to 16-byte multiples
};

// Update the number of lights to include the new light type
const int NUM_LIGHTS = 3;

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

    // Define and initialize the light array
    Light lights[NUM_LIGHTS];

    // Moonlight setup (light 0)
    lights[0].position = moonLight_position;  // High position to simulate the moon
    lights[0].color = glm::vec3(0.7f, 0.7f, 0.7f);      // Cool light blue color
    lights[0].ambientStrength = 0.6f;
    lights[0].diffuseStrength = 0.5f;
    lights[0].specularStrength = 0.5f;
    lights[0].direction = glm::vec3(0.0f, 0.0f, 0.0f);  // Not relevant for non-reflector
    lights[0].cutoffAngle = 180.0f;                     // Full spread (no cutoff)
    lights[0].flag = 0;                                 // Non-spotlight

    // Reflector light setup (light 1)
    lights[1].position = glm::vec3(-5.0f, 0.0f, 5.0f);  // Position for reflector light
    lights[1].color = reflector_color;      // Red color for reflector light
    lights[1].ambientStrength = 0.1f;
    lights[1].diffuseStrength = 0.9f;;
    lights[1].specularStrength = 0.9f;
    lights[1].direction = reflector_direction; // Direction for reflector
    lights[1].cutoffAngle = 25.0f;                      // Spotlight angle (in degrees)
    lights[1].flag = 1;                                 // Spotlight

    // Point light (light 2)
    lights[2].position = glm::vec3(-3.0f, 1.0f, -3.0f);
    lights[2].color = crystal_color;
    lights[2].ambientStrength = 0.4f;
    lights[2].diffuseStrength = 0.9f;
    lights[2].specularStrength = 0.9f;
    lights[2].direction = glm::vec3(0.0f, 0.0f, 0.0f);  // No specific direction for point light
    lights[2].cutoffAngle = 0.0f;
    lights[2].flag = 2;

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



