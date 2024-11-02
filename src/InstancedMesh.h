#pragma once

#include <string>
#include <vector>
#include <GL/glew.h>
#include <glm/vec3.hpp>
#include "tiny_obj_loader.h"

class InstancedMesh {
    struct gl_buffer {
    public:
        GLuint vao, vbo, tbo, nbo, ibo = 0;
        GLsizei size = 0;
    };


public:
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::vector<gl_buffer> buffers;
    /*!
     * Constructor that loads a mesh from an OBJ file.
     * @param obj_file - Path to the OBJ file to load
     */
    InstancedMesh(const std::string &obj_file);

    /*!
     * Destructor to clean up resources.
     */
    ~InstancedMesh();

    /*!
     * Set the positions for each instance.
     * @param instancePositions - A vector of positions for instancing
     */
    void setInstancePositions(const std::vector<glm::vec3>& instancePositions);

    /*!
     * Render the mesh with instancing.
     * @param instanceCount - The number of instances to render
     */


};
