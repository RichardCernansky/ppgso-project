#version 330
// The inputs will be fed by the vertex buffer objects
uniform vec3 Position;
uniform vec3 Color;

// Matrices as program attributes
uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;

// Passed to fragment shader
out vec3 vertexColor;

void main() {
  // Pass on the color to the fragment shader, this will be interpolated
  vertexColor = Color;

  // Calculate the final position on screen
  gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(Position, 1.0);
}
