//
// Created by Richard Čerňanský on 12/10/2024.
//
// trees.h
#pragma once  // Ensure this header is included only once during compilation

#include <glm/glm.hpp>  // Include necessary glm headers for transformation matrices


float randomFloat(float min, float max);
glm::mat4 generateRandomTreeModelMatrix();
glm::mat4 rotateToFaceDirection(const glm::vec3& base_forward, const glm::vec3& targetDirection);
void addGrassPatches(Scene &scene, glm::vec3 initControlPoints[4][4]);