#version 440 core

vec3 reinhardt_tonemap(vec3 hdr)
{
	return hdr / (1 + hdr);
}

vec3 gamma_correction(vec3 ldr)
{
	return pow(ldr, 1.0 / vec3(2.2));
}

layout(location = 0) out vec3 o_color;

in vec2 v_texcoord;

uniform float u_time;

uniform sampler2D u_albedo;
uniform sampler2D u_position;
uniform sampler2D u_normal;
uniform sampler2D u_depth;

void main()
{

	vec3 tex_col = texture(u_albedo, v_texcoord).rgb; 
	o_color = reinhardt_tonemap(tex_col);
	o_color = gamma_correction(o_color);
}