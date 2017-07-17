#version 410

out vec4 o_color;

in vec2 v_texcoord;

in vec2 v_position;

uniform sampler2D u_texture;

uniform float u_time;

uniform vec2 u_mouse;

void main()
{
	o_color = texture(u_texture, v_texcoord);
}