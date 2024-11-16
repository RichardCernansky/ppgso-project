#version 330 core

// The inputs will be fed by the vertex buffer objects
layout(location = 0) in vec3 Position; // Vertex position
layout(location = 1) in vec2 TexCoord; // Texture coordinates
layout(location = 2) in vec3 Normal;   // Vertex normal

// Matrices as program attributes
uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;

// Outputs to the fragment shader
out vec3 FragPos;       // Position of the fragment in world space
out vec3 NormalInterp;  // Interpolated normal for lighting calculations
out vec2 texCoord;      // Texture coordinates

void main() {
    // Pass the texture coordinates to the fragment shader
    texCoord = TexCoord;

    // Compute the world-space position of the fragment
    FragPos = vec3(ModelMatrix * vec4(Position, 1.0));

    // Transform the normal to world space
    NormalInterp = mat3(transpose(inverse(ModelMatrix))) * Normal;

    // Calculate the final position on screen
    gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(Position, 1.0);
}
