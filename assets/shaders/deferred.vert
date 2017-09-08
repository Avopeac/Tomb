#version 440 core

layout (location = 0) in vec3 i_position;
layout (location = 1) in vec3 i_normal;
layout (location = 2) in vec2 i_texcoord;

out gl_PerVertex
{
	vec4 gl_Position;
};

out vec2 v_texcoord;
out vec3 v_position;
out vec3 v_normal;
out vec3 v_color;

uniform mat4 u_mvp;
uniform mat4 u_mv;
uniform mat4 u_normal;
uniform vec3 u_color;

void main()
{
	gl_Position = u_mvp * vec4(i_position, 1);
	v_position = (u_mv * vec4(i_position, 1)).xyz;
	v_normal = normalize((u_normal * vec4(i_normal, 0)).xyz);
	v_texcoord = vec2(i_texcoord.x, 1 - i_texcoord.y);
	v_color = u_color;
}