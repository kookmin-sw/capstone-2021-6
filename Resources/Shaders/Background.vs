#version 450 core
layout (location = 0) in vec3 aPos;

uniform mat4 projection;
unifrom mat4 view;

out vec3 worldPos;

void main()
{
    worldPos = aPos;
    mat4 rotView = mat4(mat3(view));
    vec3 clipPos = projection * rotView * vec4(worldPos, 1.0);

    gl_Position = clipPos.xyww;
}