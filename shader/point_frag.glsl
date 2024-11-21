#version 330 core

// Uniform for the color
uniform vec3 Color;

// Output color to the framebuffer
out vec4 FragColor;

void main() {
    // Set the fragment color to the uniform value
    FragColor = vec4(Color, 1.0); // Use uniform color with full opacity
}