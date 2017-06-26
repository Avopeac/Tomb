#version 430

out vec4 o_color;

in vec2 v_texcoord;

in vec2 v_position;

uniform sampler2D u_texture;

uniform float u_time;

void main()
{
	vec2 texcoord_wobble = 0.005 * vec2(0.0, sin(5.0 * u_time + v_position.y));
	o_color = texture(u_texture, v_texcoord + texcoord_wobble);
}