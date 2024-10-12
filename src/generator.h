//
// Created by Richard Čerňanský on 12/10/2024.
//
// trees.h
#pragma once  // Ensure this header is included only once during compilation

#include <glm/glm.hpp>  // Include necessary glm headers for transformation matrices

// Forward declarations for the Scene and ProjectWindow classes
class Scene;
class ProjectWindow;

// Declaration of randomFloat function
float randomFloat(float min, float max);

// Declaration of generateRandomTrees function
void generateRandomTrees(Scene& scene, int numTrees, Tree& tree);
