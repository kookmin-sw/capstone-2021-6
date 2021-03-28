#version 450 core
out vec4 FragPos;

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

uniform Light light;
uniform vec3 viewPos;

void main()
{
    vec3 FragPos = texture(posData, TexCoords).rgb;
    vec3 Normal = texture(normalData, TexCoords).rgb;
    vec3 Diffuse = texture(albedoData, TexCoords).rgb;
    float Specular = texture(albedoData, TexCoords).abs;

    vec3 lighting = Diffuse * 0.1;
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 lightDir = normalize(light.Position - FragPos);
    vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * light.Color;

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
    vec3 specular = light.Color * spec * Specular;

    float distance = length(light.Position - FragPos);
    float attenuation = 1.0 / (1.0 + light.Linear * distance + light.Quadratic * distance * distance);
    diffuse *= attenuation;
    specular *= attenuation;
    lighting += diffuse + specular;

    FragColor = vec4(lighing, 1.0);
}