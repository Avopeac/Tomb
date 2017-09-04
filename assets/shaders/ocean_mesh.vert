#version 440 core

layout (location = 0) in vec3 i_position;
layout (location = 1) in vec3 i_normal;
layout (location = 2) in vec2 i_texcoord;

out gl_PerVertex
{
	vec4 gl_Position;
};

out vec2 v_texcoord;
out vec3 v_direction;
out vec3 f_normal;
out flat vec3 f_sun;
out vec4 f_color;

uniform mat4 u_mvp;
uniform mat4 u_view;
uniform mat4 u_vp;
uniform mat4 u_normal;
uniform vec4 u_color;

void main()
{
	gl_Position = u_mvp * vec4(i_position, 1);

	v_direction = normalize((u_view * vec4(i_position, 1)).xyz);
	v_texcoord = i_texcoord;
	f_color = u_color;
	f_sun = normalize((u_view * vec4(0, 1, -0.5, 0.0)).xyz);
	f_normal = normalize((u_normal * vec4(i_normal, 0)).xyz);
}