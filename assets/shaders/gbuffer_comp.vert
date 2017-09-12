#version 440 core

layout (location = 0) in vec2 i_position;

out gl_PerVertex
{
	vec4 gl_Position;
};

out vec2 v_texcoord;
out vec3 v_sun;

uniform mat4 u_view;

void main()
{

	gl_Position = vec4(2.0 * i_position - 1.0, 0, 1);
	v_texcoord = 0.5 + 0.5 * gl_Position.xy;
	v_texcoord.y = 1.0 - v_texcoord.y;
	v_sun = (u_view * vec4(0, 1.0, -0.1, 0)).xyz;
}