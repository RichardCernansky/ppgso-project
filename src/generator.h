#pragma once

#include <glm/glm.hpp>

float randomFloat(float min, float max);
std::pair<glm::mat4, glm::mat4> generateRandomTreeModelMatrix();
std::pair<glm::mat4, glm::mat4> generateRandomTreeModelMatrixAlternative();
glm::mat4 rotateToFaceDirection(const glm::vec3& base_forward, const glm::vec3& targetDirection);
GLuint set_up_lights(GLuint shaderProgram);