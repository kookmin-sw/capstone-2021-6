#version 450 core
layout (location = 0) in vec3 aPos;

out vec3 WorldPos;

uniform mat4 projMatrix;
uniform mat4 viewMatrix;

void main()
{
	WorldPos = aPos;
	gl_Position = projMatrix * viewMatrix * vec4(WorldPos, 1.0);
}