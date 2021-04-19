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

uniform bool b_shadows;

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

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / max(denom, 0.0000001);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}

void main()
{
    vec3 FragPos = texture(posData, TexCoords).rgb;
    vec3 Normal = texture(normalData, TexCoords).rgb;
    vec3 Albedo = texture(albedoData, TexCoords).rgb;
    float Specular = texture(albedoData, TexCoords).a;

    vec3 N = normalize(Normal);
    vec3 V = normalize(camPos - FragPos);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, Albedo, metallic);

    vec3 Lo = vec3(0.0);

    float shadow = b_shadows ? ShadowCalculation(FragPos) : 0.0;

    for(int i = 0; i < 4; ++i)
    {
        vec3 L = normalize(light[i].Position - FragPos);
        vec3 H = normalize(V + L);
        float distance = length(light[i].Position - FragPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = light[i].Color * attenuation;

        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        vec3 F = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);

        vec3 nominator = NDF * G * F;
        float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
        vec3 specular = (nominator / max(denominator, 0.001));

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;

        kD *= 1.0 - metallic;

        float NdotL = max(dot(N, L), 0.0);

        Lo += (kD * Albedo / PI + specular) * radiance * NdotL;
    }

    vec3 ambient = vec3(0.03) * Albedo * ao;

    vec3 color = ambient + (1.0 - shadow) * Lo;

    color = color / (color + vec3(1.0));

    color = pow(color, vec3(1.0/2.2));

    FragColor = vec4(color, 1.0);
}