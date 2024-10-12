//
// Created by Richard Čerňanský on 12/10/2024.
//
//my files
#include "src/window.cpp"
#include <glm/fwd.hpp>
#include <random>
#include "objects/tree.h"

float randomFloat(float min, float max) {
    // Create a random number generator
    static std::random_device rd;  // Seed for random number engine
    static std::mt19937 gen(rd());  // Mersenne Twister engine for generating random numbers

    // Create a uniform real distribution for floating-point numbers in the given range
    std::uniform_real_distribution<float> dist(min, max);

    // Return a random floating-point number in the range [min, max]
    return dist(gen);
}
void generateRandomTrees(Scene& scene, int numTrees, Tree& tree) {
    tree.instanceTransforms.clear();  // Clear any existing transforms

    for (int i = 0; i < numTrees; ++i) {
        // Generate random position for the tree
        float x = randomFloat(-20.0f, 20.0f);
        float z = randomFloat(-20.0f, 20.0f);
        float y = 0.0f;

        // Generate random scale for the tree
        float scale = randomFloat(0.05f, 0.3f);

        // Create transformation matrix
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
        model = glm::scale(model, glm::vec3(scale, scale, scale));

        // Add the matrix to the list
        tree.instanceTransforms.push_back(model);
    }
}
