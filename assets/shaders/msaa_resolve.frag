#version 440 core

vec3 reinhardt_tonemap(vec3 hdr)
{
	return hdr; //hdr / (1 + hdr);
}

vec3 inv_reinhardt_tonemap(vec3 ldr) {
	//return -ldr / (ldr - 1);
	//return ldr / (1 - ldr);
	return ldr;
}

layout(location = 0) out vec4 o_color;
layout(location = 1) out float o_depth;

uniform sampler2DMS u_texture0;
uniform sampler2DMS u_depth_texture;

uniform ivec2 u_resolution;
uniform int u_num_samples;

void main()
{

	vec4 color = vec4(0.0);
	float depth = 0.0;

	for (int i = 0; i < u_num_samples; ++i) {

		vec4 tex_col = texelFetch(u_texture0, ivec2(gl_FragCoord.xy), i); 
		
		//color.rgb += reinhardt_tonemap(tex_col.rgb);
		color.rgb += tex_col.rgb;
		color.a += tex_col.a;

		depth += texelFetch(u_depth_texture, ivec2(gl_FragCoord.xy), i).r;
	}	

	//o_color.rgb = inv_reinhardt_tonemap(color.rgb / float(u_num_samples));
	o_color.rgb = color.rgb / float(u_num_samples);
	o_color.a = color.a / float(u_num_samples);
	o_depth = depth / float(u_num_samples);
}