#version 440 core

vec3 reinhardt_tonemap(vec3 hdr)
{
	return hdr / (1 + hdr);
}

vec3 gamma_correction(vec3 ldr)
{
	return pow(ldr, 1.0 / vec3(2.2));
}

layout(location = 0) out vec4 o_color;

in vec2 v_texcoord;

uniform float u_time;
uniform sampler2D u_texture0;
uniform sampler2D u_texture1;

void main()
{
	vec4 tex_col = texture(u_texture0, v_texcoord); 
	o_color.rgb = reinhardt_tonemap(tex_col.rgb);
	o_color.rgb = gamma_correction(o_color.rgb);
}