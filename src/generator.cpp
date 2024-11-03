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


void addGrassPatches(Scene &scene, glm::vec3 initControlPoints[4][4]) {
    srand(static_cast<unsigned int>(time(nullptr)));  // Seed for random number generation

    for (int i = 0; i < 10; i++) {
        // Generate a random offset in the range of ±0.2
        float offsetX = (static_cast<float>(rand()) / RAND_MAX) * 0.4f - 0.2f;
        float offsetY = 0;
        float offsetZ = (static_cast<float>(rand()) / RAND_MAX) * 0.4f - 0.2f;

        // Create a new GrassPatch with the initial control points
        auto grass = std::make_unique<GrassPatch>(initControlPoints);
        // Set its position to the default position plus the random offset
        grass->position += glm::vec3(offsetX, offsetY, offsetZ);

        // Add the grass object to the scene
        scene.objects.push_back(std::move(grass));
    }
}

