#version 430

out vec4 o_color;

in vec2 v_texcoord;

in vec2 v_position;

flat in uint v_texture_array_index;

uniform sampler2DArray u_texture;

uniform float u_time;

void main()
{
	//vec4 color = texture(u_texture, vec3(v_texcoord, float(v_texture_array_index)));

	vec4 color = texture(u_texture, vec3(v_texcoord, float(0)));

	o_color = vec4(1,1,1,color.a);
}