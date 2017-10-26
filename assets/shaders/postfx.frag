#version 440 core

layout(location = 0) out vec4 o_color;

in vec2 v_texcoord;

uniform float u_time;
uniform vec2 u_texel_size;
uniform sampler2D u_texture0;

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
	vec3 color = texture(u_texture0, v_texcoord).rgb;
	o_color = vec4(gamma_correction(reinhardt_tonemap(color)), 1.0);
}