#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

void main()
{
	vec4 worldPos = modelMatrix * vec4(aPos, 1.0);
	FragPos = worldPos.xyz;
	TexCoords = aTexCoords;

	mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
	Normal = normalMatrix * aNormal;

	gl_Position = projMatrix * viewMatrix * worldPos;
}
