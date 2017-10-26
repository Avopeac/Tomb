#version 440 core

layout(location = 0) out vec4 o_color;

in vec2 v_texcoord;

uniform float u_time;
uniform vec2 u_texel_size;
uniform sampler2D u_texture0;
uniform sampler2D u_shadow;

vec3 reinhardt_tonemap(vec3 hdr)
{
	return hdr / (1 + hdr);
}

vec3 gamma_correction(vec3 ldr)
{
	return pow(ldr, 1.0 / vec3(2.2));
}

void main()
{
	vec4 tex_col = vec4(texture(u_texture0, v_texcoord).rgb, 1.0); 
	o_color = tex_col;
}