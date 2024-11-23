#version 330 core
in vec2 texCoord;
out vec4 FragColor;

uniform sampler2D image;
uniform float weight[5];

void main()
{
    vec2 tex_offset = vec2(1.0 / textureSize(image, 0).x, 0.0); // Horizontal offset
    vec3 result = texture(image, texCoord).rgb * weight[0];

    for (int i = 1; i < 5; ++i)
    {
        result += texture(image, texCoord + tex_offset * float(i)).rgb * weight[i];
        result += texture(image, texCoord - tex_offset * float(i)).rgb * weight[i];
    }
    FragColor = vec4(result, 1.0);
}
