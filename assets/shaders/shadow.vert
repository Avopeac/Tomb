#version 440 core

layout (location = 0) in vec3 i_position;
layout (location = 1) in vec3 i_normal;
layout (location = 2) in vec2 i_texcoord;

out gl_PerVertex
{
	vec4 gl_Position;
};

uniform mat4 u_mvp;

void main()
{
	gl_Position = u_mvp * vec4(i_position, 1);
}