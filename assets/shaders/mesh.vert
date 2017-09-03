#version 440 core

layout (location = 0) in vec3 i_position;
layout (location = 1) in vec3 i_normal;
layout (location = 2) in vec2 i_texcoord;

out gl_PerVertex
{
	vec4 gl_Position;
};

out flat vec3 v_normal;
out vec2 v_texcoord;

out flat vec3 f_sun;

uniform mat4 u_mvp;
uniform mat4 u_view;
uniform mat4 u_normal;

void main()
{
	gl_Position = u_mvp * vec4(i_position, 1);

	v_normal = normalize((u_normal * vec4(i_normal, 0)).xyz);
	v_texcoord = i_texcoord;

	f_sun = normalize((u_view * vec4(0, 1, -0.5, 0.0)).xyz);
}