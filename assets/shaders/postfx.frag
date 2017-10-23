#version 440 core

layout(location = 0) out vec4 o_color;

in vec2 v_texcoord;

uniform float u_time;
uniform vec2 u_texel_size;
uniform sampler2D u_texture0;
uniform sampler2D u_shadow;

#define FXAA_SPAN_MAX	2.0
#define FXAA_REDUCE_MUL 1.0/2.0
#define FXAA_REDUCE_MIN 1.0/128.0

vec3 reinhardt_tonemap(vec3 hdr)
{
	return hdr / (1 + hdr);
}

vec3 gamma_correction(vec3 ldr)
{
	return pow(ldr, 1.0 / vec3(2.2));
}

vec3 fxaa() {
	vec3 luma = vec3(0.299, 0.587, 0.114);
	vec3 nw = texture(u_texture0, v_texcoord + vec2(-u_texel_size.x, u_texel_size.y)).rgb;
	vec3 ne = texture(u_texture0, v_texcoord + vec2(u_texel_size.x, u_texel_size.y)).rgb;
	vec3 m = texture(u_texture0, v_texcoord).rgb;
	vec3 sw = texture(u_texture0, v_texcoord + vec2(-u_texel_size.x, -u_texel_size.y)).rgb;
	vec3 se = texture(u_texture0, v_texcoord + vec2(u_texel_size.x, -u_texel_size.y)).rgb;

	float luma_nw = dot(luma, nw);
	float luma_ne = dot(luma, ne);
	float luma_m = dot(luma, m);
	float luma_sw = dot(luma, sw);
	float luma_se = dot(luma, se);
	float luma_min = min(luma_nw, min(luma_ne, min(luma_m, min(luma_sw, luma_se))));
	float luma_max = max(luma_nw, max(luma_ne, max(luma_m, max(luma_sw, luma_se))));
	
	vec2 dir;
	dir.x = -((luma_nw + luma_ne) - (luma_sw + luma_se));
	dir.y = ((luma_nw + luma_sw) - (luma_ne + luma_se));

	float dir_reduce = max((luma_nw + luma_ne + luma_sw + luma_se) * (0.25 * FXAA_REDUCE_MUL), FXAA_REDUCE_MIN);
	float rcp_dir_min = 1.0 / (min(abs(dir.x), abs(dir.y)) + dir_reduce);


	dir.x = min(FXAA_SPAN_MAX, max(-FXAA_SPAN_MAX, dir.x * rcp_dir_min)) * u_texel_size.x;
	dir.y = min(FXAA_SPAN_MAX, max(-FXAA_SPAN_MAX, dir.y * rcp_dir_min)) * u_texel_size.y;

	vec3 rgb_a = (1.0 / 2.0) * (texture(u_texture0, v_texcoord + dir * (1.0 / 3.0 - 0.5)).rgb +
								texture(u_texture0, v_texcoord + dir * (1.0 / 1.0 - 0.5)).rgb);

	vec3 rgb_b = rgb_a * (1.0 / 2.0) + (1.0 / 4.0) * (texture(u_texture0, v_texcoord + dir * (0.0 / 3.0 - 0.5)).rgb + 
													  texture(u_texture0, v_texcoord + dir * (3.0 / 3.0 - 0.5)).rgb);

	vec3 color; 

	float luma_b = dot(rgb_b, luma);
	if ((luma_b < luma_min) || (luma_b > luma_max)) 
	{
		color = rgb_a;
	} else {
		color = rgb_b;
	}

	return color;
}

void main()
{
	vec4 tex_col = vec4(fxaa(), 1.0); 
	o_color = tex_col;
}