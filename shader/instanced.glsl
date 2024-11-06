#version 330 core
layout(location = 0) in vec3 Position;
layout(location = 1) in vec2 TexCoord;
layout(location = 2) in vec3 instancePosition; // New instance position input

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;

out vec2 texCoord;

void main() {
    texCoord = TexCoord;
    // Add instance position to vertex position
    gl_Position = ProjectionMatrix * ViewMatrix * vec4(Position + instancePosition, 1.0);
}
