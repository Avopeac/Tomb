#version 410

out vec4 o_color;

in vec2 v_texcoord;

uniform sampler2D u_texture;

uniform float u_time;

uniform vec4 u_color;

void main()
{
	vec4 color = texture(u_texture, v_texcoord);
	o_color = u_color;
	o_color.rgb *= color.a;

	//o_color = u_color * texture(u_texture, v_texcoord).a;
}