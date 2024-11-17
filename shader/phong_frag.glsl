#version 330 core

// Define the number of lights
#define NUM_LIGHTS 1

// Inputs from the vertex shader
in vec3 FragPos;       // Position of the fragment in world space
in vec3 NormalInterp;  // Interpolated normal for lighting calculations
in vec2 texCoord;      // Texture coordinates

// Output color
out vec4 FragmentColor;

// Light source struct
struct Light {
    vec3 position;
    float ambientStrength; // Padding for std140 alignment
    vec3 color;
    float diffuseStrength;
    float specularStrength;
    float padding;         // Additional padding for std140 alignment
};

// Uniform block with a fixed-size array of light sources
layout(std140) uniform LightBlock {
    Light lights[NUM_LIGHTS]; // Fixed-size array for light sources
};

// Uniforms for camera/view position and texture sampler
uniform vec3 viewPos;             // Camera/view position
uniform sampler2D texture1;       // Texture sampler

// Static material properties
const vec3 materialDiffuse = vec3(1.0, 1.0, 1.0);    // Static diffuse color
const vec3 materialSpecular = vec3(0.5, 0.5, 0.5);   // Static specular color
const float materialShininess = 32.0;                // Static shininess factor

void main() {
    // Sample the texture color
    vec3 texColor = texture(texture1, texCoord).rgb;

    // Initialize lighting components
    vec3 ambient = vec3(0.0);
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);

    // Normalize the normal vector
    vec3 norm = normalize(NormalInterp);

    // Iterate over each light source (only one in this case)
    for (int i = 0; i < NUM_LIGHTS; ++i) {
        // Ambient component
        ambient += lights[i].ambientStrength * lights[i].color;

        // Diffuse component
        vec3 lightDir = normalize(lights[i].position - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        diffuse += lights[i].diffuseStrength * diff * lights[i].color;

        // Specular component
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), materialShininess);
        specular += lights[i].specularStrength * spec * lights[i].color;
    }

    // Combine all lighting components
    vec3 result = (ambient + diffuse + specular) * texColor;
    FragmentColor = vec4(result, 1.0);
}
