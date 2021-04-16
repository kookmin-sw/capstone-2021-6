#version 450 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D posData;
uniform sampler2D normalData;
uniform sampler2D albedoData;

uniform samplerCube depthMap;

uniform float far_plane;

uniform float metallic;
uniform float roughness;
uniform float ao;

struct Light {
    vec3 Position;
    vec3 Color;
};

const int lightNum = 4;
uniform Light light[lightNum];

uniform vec3 camPos;

const float PI = 3.14159265359;

vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

float ShadowCalculation(vec3 fragPos)
{
    vec3 fragToLight = fragPos - light[0].Position;

    float currentDepth = length(fragToLight);

    float shadow = 0.0;
    float bias = 0.15;
    int samples = 20;
    float viewDistance = length(camPos - fragPos);
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(depthMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= far_plane;   
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);

    return shadow;
}

void main()
{
    vec3 color = texture(albedoData, TexCoords).rgb;
    vec3 normal = normalize(texture(normalData, TexCoords).rgb);
    vec3 lightColor = vec3(10.0);

    vec3 ambient = 0.3 * color;

    vec3 lightDir = normalize(light[0].Position - texture(posData, TexCoords).rgb);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * light[0].Color;

    vec3 viewDir = normalize(camPos - texture(posData, TexCoords).rgb);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;

    float shadow = ShadowCalculation(texture(posData, TexCoords).rgb);
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;

    FragColor = vec4(lighting, 1.0);
}