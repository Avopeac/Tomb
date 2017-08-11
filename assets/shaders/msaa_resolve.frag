#version 440 core

layout(location = 0) out vec4 o_color;
layout(location = 1) out float o_depth;

in vec2 v_texcoord;
in vec2 v_position;

uniform sampler2DMS u_color_attach0;
uniform sampler2DMS u_depth_attach;

uniform ivec2 u_resolution;
uniform int u_num_samples;

void main()
{

	vec4 color = vec4(0.0);
	float depth = 0.0;

	for (int i = 0; i < u_num_samples; ++i) {
		color += texelFetch(u_color_attach0, ivec2(gl_FragCoord.xy), i);
		depth += texelFetch(u_depth_attach, ivec2(gl_FragCoord.xy), i).r;
	}	

	o_color = color;
	o_depth = depth;
}