#version 440 core

in vec2 v_texcoord;
in vec3 v_position;
in vec3 v_normal;
in vec3 v_color;
in vec4 v_shadowcoord;

layout(location = 0) out vec3 o_albedo;
layout(location = 1) out vec3 o_position;
layout(location = 2) out vec3 o_normal;
layout(location = 3) out float o_shadow;
layout(location = 4) out vec2 o_motion;

uniform sampler2D u_texture;
uniform sampler2D u_shadow;

float computeVisibility() {
	const float z_bias = 0.005;
	return texture(u_shadow, v_shadowcoord.xy).r < v_shadowcoord.z - z_bias ? 0.0 : 1.0; 
}

void main()
{
	o_albedo = v_color;
	o_position = v_position;
	o_normal = normalize(v_normal);
	o_shadow = computeVisibility();
}