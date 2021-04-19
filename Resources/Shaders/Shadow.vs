#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 modelMatrix;

void main()
{
	gl_Position = modelMatrix * vec4(aPos, 1.0);
}