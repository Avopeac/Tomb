#version 440 core

layout(location = 0) out vec2 o_vsm_depth;

in vec2 v_texcoord;

uniform sampler2D u_shadow;

void main()
{
	float depth = texture(u_shadow, v_texcoord).r; 
	vec2 depth_differential = vec2(dFdx(depth), dFdy(depth));
	o_vsm_depth = vec2(depth, depth * depth + 0.25 * (dot(depth_differential, depth_differential)));
}