#version 330 core
#define NUM_LIGHTS 3  // Total number of lights

// Inputs from the vertex shader
in vec3 FragPos;
in vec3 NormalInterp;
in vec2 texCoord;
in vec3 ViewPos;

// Output color
out vec4 FragmentColor;

struct Light {
    vec3 position;       // vec3 aligned to 16 bytes
    float ambientStrength;

    vec3 color;          // vec3 aligned to 16 bytes
    float diffuseStrength;

    vec3 direction;      // vec3 aligned to 16 bytes
    float specularStrength;

    float cutoffAngle;   // Single float aligned to 4 bytes
    int flag;            // Integer aligned to 4 bytes
    int padding1;        // Padding to maintain 16-byte alignment
    int padding2;        // Additional padding to align struct size to 16 bytes
};

layout(std140) uniform LightBlock {
    Light lights[NUM_LIGHTS]; // Fixed-size array for light sources
};

uniform sampler2D Texture;
uniform float Transparency;
uniform vec2 TextureOffset;

const float kc = 1.0;      // Constant attenuation factor
const float kl = 0.9;     // Linear attenuation factor
const float kq = 0.32;    // Quadratic attenuation factor

// Material properties for ordinary objects
const vec3 materialDiffuse = vec3(0.8, 0.3, 0.3);    // A reddish color for an apple
const vec3 materialSpecular = vec3(0.1, 0.1, 0.1);   // Moderate specular reflection
const float materialShininess = 4.0;                 // Medium shininess for a smooth surface

void main() {
    vec3 result = vec3(0.0); // Final color
    vec3 texColor = texture(Texture, vec2(texCoord.x, 1.0 - texCoord.y) + TextureOffset).rgb;

    for (int i = 0; i < NUM_LIGHTS; i++) {
        Light light = lights[i];

        // Calculate distance to the current light
        float distance = length(light.position - FragPos);

        // Normal light processing
        float attenuation = 1.0;
        if ( light.flag == 2) { // Spotlight or Point Light
            attenuation = 1.0 / (kc + kl * distance + kq * (distance * distance));
        }

        // Ambient component
        vec3 ambient = light.ambientStrength * light.color * attenuation;

        // Diffuse component
        vec3 norm = normalize(NormalInterp);
        vec3 lightDir = normalize(light.position - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = light.diffuseStrength * diff * light.color * attenuation;

        // Specular component
        vec3 viewDir = normalize(ViewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), materialShininess);
        vec3 specular = light.specularStrength * spec * light.color * attenuation;

        // Spotlight cutoff (only for flag == 1)
        if (light.flag == 1) {
            vec3 spotDir = normalize(-light.direction); // Direction the spotlight is pointing
            float theta = dot(lightDir, spotDir);
            float epsilon = cos(radians(light.cutoffAngle));
            if (theta < epsilon) {
                diffuse = vec3(0.0);
                specular = vec3(0.0);
            }
        }

        result += ambient + diffuse + specular;
    }


    FragmentColor = vec4(result * texColor, Transparency);
}
