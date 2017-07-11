#version 410

out vec4 o_color;

in vec2 v_texcoord;

in vec2 v_position;

flat in uint v_texture_array_index;

uniform sampler2DArray u_texture;

uniform float u_time;

uniform vec4 u_color;

void main()
{
	//vec4 color = texture(u_texture, vec3(v_texcoord, v_texture_array_index));
	//o_color = u_color;
	//o_color.rgb *= color.a;

	o_color = u_color * texture(u_texture, vec3(v_texcoord, v_texture_array_index)).a;
}