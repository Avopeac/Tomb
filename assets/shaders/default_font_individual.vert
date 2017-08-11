#version 440 core

layout (location = 0) in vec2 i_position;

out gl_PerVertex
{
	vec4 gl_Position;
};

out vec2 v_texcoord;

uniform mat4 u_viewproj;

uniform float u_time;

uniform vec2 u_position;

uniform float u_xoffset;

uniform vec2 u_size;

void main()
{
	vec2 position = u_position + i_position * u_size;
	position.x += u_xoffset;

	gl_Position = u_viewproj * vec4(position, 0, 1);

	v_texcoord = 0.5 + 0.5 * i_position.xy;
	v_texcoord.y = 1.0 - v_texcoord.y;
}