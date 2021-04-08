#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;

out vec2 TexCoords;

uniform mat4 projMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

void main()
{
	TexCoords = aTexCoords;
	vec3 WorldPos = vec3(modelMatrix * vec4(aPos, 1.0));

	gl_Position = projMatrix * viewMatrix * vec4(WorldPos, 1.0);
}