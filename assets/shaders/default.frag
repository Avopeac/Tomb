#version 430

out vec4 o_color;

in vec2 v_texcoord;

uniform sampler2D u_texture;

void main()
{
	//o_color = texture(u_texture, v_texcoord);

	o_color = vec4(1,0,1,0.1);

}