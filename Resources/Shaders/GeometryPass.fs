#version 450 core
layout (location = 0) out vec3 posData;
layout (location = 1) out vec3 normalData;
layout (location = 2) out vec4 albedoData;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

void main()
{
    posData = FragPos;
    normalData = normalize(Normal);
    albedoData.rgb = texture(texture_diffuse1, TexCoords).rgb;
    albedoData.a = texture(texture_specular1, TexCoords).r;
}

