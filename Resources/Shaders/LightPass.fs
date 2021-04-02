#version 450 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D posData;
uniform sampler2D normalData;
uniform sampler2D albedoData;

struct Light {
    vec3 Position;
    vec3 Color;

    float Linear;
    float Quadratic;
};

const int lightNum = 32;
uniform Light light[lightNum];
uniform vec3 viewPos;

void main()
{
    vec3 FragPos = texture(posData, TexCoords).rgb;
    vec3 Normal = texture(normalData, TexCoords).rgb;
    vec3 Diffuse = texture(albedoData, TexCoords).rgb;
    float Specular = texture(albedoData, TexCoords).a;

    vec3 lighting = Diffuse * 0.1;
    vec3 viewDir = normalize(viewPos - FragPos);

    for (int i = 0; i < lightNum; ++i)
    {
        vec3 lightDir = normalize(light[i].Position - FragPos);
        vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * light[i].Color;

        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
        vec3 specular = light[i].Color * spec * Specular;

        float distance = length(light[i].Position - FragPos);
        float attenuation = 1.0 / (1.0 + light[i].Linear * distance + light[i].Quadratic * distance * distance);
        diffuse *= attenuation;
        specular *= attenuation;
        lighting += diffuse + specular;
    }

    FragColor = vec4(lighting, 1.0);
}