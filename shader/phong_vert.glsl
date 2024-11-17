#version 330 core

layout(location = 0) in vec3 Position; // Vertex position
layout(location = 1) in vec2 TexCoord; // Texture coordinates
layout(location = 2) in vec3 Normal;   // Vertex normal

// Matrices as program attributes
uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;

// Outputs to the fragment shader
out vec3 FragPos;          // Position of the fragment in world space
out vec3 NormalInterp;     // Interpolated normal for lighting calculations
out vec2 texCoord;         // Texture coordinates
out vec3 ViewPos;          // Camera position in world space for the fragment shader

void main() {
    texCoord = TexCoord;
    FragPos = vec3(ModelMatrix * vec4(Position, 1.0));
    NormalInterp = mat3(transpose(inverse(ModelMatrix))) * Normal;
    NormalInterp = normalize(NormalInterp);

    // Pass the camera/view position in world space to the fragment shader
    ViewPos = vec3(inverse(ViewMatrix)[3]); // Extract the camera position from the inverse of the ViewMatrix

    gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(Position, 1.0);
}
