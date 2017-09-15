#version 440 core

in vec2 v_texcoord;
in vec3 v_position;
in vec3 v_normal;
in vec3 v_color;

layout(location = 0) out vec3 o_albedo;
layout(location = 1) out vec3 o_position;
layout(location = 2) out vec3 o_normal;

uniform sampler2D u_texture;

void main()
{
	vec3 tex_col = texture(u_texture, v_texcoord).rgb;
	o_albedo = tex_col * v_color;
	o_position = v_position;
	o_normal = normalize(v_normal);
}