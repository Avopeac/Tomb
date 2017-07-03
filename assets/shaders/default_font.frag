#version 430

out vec4 o_color;

in vec2 v_texcoord;

in vec2 v_position;

flat in uint v_texture_array_index;

uniform sampler2DArray u_texture;

uniform float u_time;

uniform vec4 u_color;

void main()
{
	o_color = u_color * texture(u_texture, vec3(v_texcoord, v_texture_array_index)).a;
}