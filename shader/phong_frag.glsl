#version 330 core

#define NUM_LIGHTS 3  // Adjust to the number of lights you have

// Inputs from the vertex shader
in vec3 FragPos;
in vec3 NormalInterp;
in vec2 texCoord;
in vec3 ViewPos;

// Output color
out vec4 FragmentColor;

// Define the Light struct
struct Light {
    vec3 position;
    float padding1;        // Padding for std140 alignment
    vec3 color;
    float ambientStrength;
    float diffuseStrength;
    float specularStrength;
    float padding2;        // Padding for std140 alignment
    vec3 direction;        // Direction for spotlight
    float cutoffAngle;     // Spotlight cutoff angle (in degrees)
    int flag;              // 0 for non-spotlight, 1 for spotlight
    float padding3;        // Padding to maintain alignment
};

// Uniform block with a fixed-size array of light sources
layout(std140) uniform LightBlock {
    Light lights[NUM_LIGHTS]; // Fixed-size array for light sources
};

// Uniforms for camera/view position and texture sampler
uniform sampler2D Texture;
uniform float Transparency;
uniform vec2 TextureOffset;

// Material properties for ordinary objects
const vec3 materialDiffuse = vec3(0.8, 0.3, 0.3);    // A reddish color for an apple
const vec3 materialSpecular = vec3(0.5, 0.5, 0.5);   // Moderate specular reflection
const float materialShininess = 16.0;                // Medium shininess for a smooth surface

void main() {
    // Sample the texture color with offset and vertical inversion for OBJ compatibility
    vec3 texColor = texture(Texture, vec2(texCoord.x, 1.0 - texCoord.y) + TextureOffset).rgb;

    // Initialize lighting components
    vec3 ambient = vec3(0.0);
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);

    // Normalize the normal vector
    vec3 norm = normalize(NormalInterp);

    // Spotlight attenuation factors
    const float k_c = 1.0;  // Constant attenuation
    const float k_l = 0.09; // Linear attenuation
    const float k_q = 0.032; // Quadratic attenuation

    // Visibility radius for the light source
    const float lightSourceVisibilityRadius = 0.5;

    // Check if the fragment is close to any light source
    bool isLightSource = false;
    vec3 lightSourceColor = vec3(0.0);

    for (int i = 0; i < NUM_LIGHTS; ++i) {
        // Calculate distance between the fragment and the light source position
        float distanceToLight = length(FragPos - lights[i].position);

        // If the fragment is close to the light source, make it emit light
        if (distanceToLight < lightSourceVisibilityRadius) {
            isLightSource = true;
            lightSourceColor = lights[i].color; // Use the light's color for the emission
            break; // No need to check other lights
        }

        // Ambient component
        ambient += lights[i].ambientStrength * lights[i].color;

        // Calculate the light direction and distance
        vec3 lightDir = normalize(lights[i].position - FragPos);
        float distance = length(lights[i].position - FragPos);
        float attenuation = 1.0 / (k_c + k_l * distance + k_q * (distance * distance));

        float diff = max(dot(norm, lightDir), 0.0);

        // Check if the light is a spotlight
        if (lights[i].flag == 1) {
            // Spotlight calculations
            float theta = dot(lightDir, normalize(-lights[i].direction));
            float epsilon = cos(radians(lights[i].cutoffAngle));

            // Implement spotlight intensity falloff
            if (theta > epsilon) {
                ambient = vec3(0.0);
                diffuse = vec3(0.0);
                specular = vec3(0.0);
                float intensity = smoothstep(epsilon, 1.0, theta);
                diffuse += attenuation * intensity * lights[i].diffuseStrength * diff * lights[i].color;

                // Specular component
                vec3 reflectDir = reflect(-lightDir, norm);
                vec3 targetDir = normalize(FragPos - lights[i].position);
                float spec = pow(max(dot(targetDir, reflectDir), 0.0), materialShininess);
                specular += attenuation * intensity * lights[i].specularStrength * spec * lights[i].color;
                break;
            }
        } else if (lights[i].flag == 2) {
            // Point light calculations with attenuation
            diffuse += attenuation * lights[i].diffuseStrength * diff * lights[i].color;

            // Specular component
            vec3 viewDir = normalize(ViewPos - FragPos);
            vec3 reflectDir = reflect(-lightDir, norm);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), materialShininess);
            specular += attenuation * lights[i].specularStrength * spec * lights[i].color;
        } else {
            // Non-spotlight light calculations
            diffuse += lights[i].diffuseStrength * diff * lights[i].color;

            // Specular component
            vec3 viewDir = normalize(ViewPos - FragPos);
            vec3 reflectDir = reflect(-lightDir, norm);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), materialShininess);
            specular += lights[i].specularStrength * spec * lights[i].color;
        }
    }

    // If this fragment is part of the light source, make it emit light
    if (isLightSource) {
        FragmentColor = vec4(lightSourceColor, 1.0); // Emit the light source color
        return; // Skip further lighting calculations
    }

    // Combine all lighting components and apply the texture color
    vec3 result = (ambient + diffuse + specular) * texColor;

    // Set the final color with transparency applied
    FragmentColor = vec4(result, Transparency);
}
