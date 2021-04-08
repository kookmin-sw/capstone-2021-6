#version 450 core
layout (location = 0) in vec3 aPos;

uniform mat4 projMatrix;
uniform mat4 viewMatrix;

out vec3 WorldPos;

void main()
{
	WorldPos = aPos;

	mat4 rotView = mat4(mat3(viewMatrix));
	vec4 clipPos = projMatrix * rotView * vec4(WorldPos, 1.0);

	gl_Position = clipPos.xyww;
}