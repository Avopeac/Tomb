#version 440 core

layout (location = 0) in vec3 i_position;
layout (location = 1) in vec3 i_normal;
layout (location = 2) in vec2 i_texcoord;

layout(std430, binding = 0) buffer transformLayout
{
	mat4 world_transforms[];
};

out gl_PerVertex
{
	vec4 gl_Position;
};

out vec2 v_texcoord;
out vec3 v_position;
out vec3 v_normal;
out vec3 v_color;
out vec4 v_shadowcoord;

uniform mat4 u_vp;
uniform mat4 u_shadow_vp;
uniform mat4 u_view;
uniform mat4 u_normal;
uniform vec3 u_color;

void main()
{
	mat4 mv = u_view * world_transforms[gl_InstanceID];
	mat3 normal = inverse(transpose(mat3(mv)));

	vec4 world_pos = world_transforms[gl_InstanceID] * vec4(i_position, 1);
	vec4 clip_pos = u_vp * world_pos;

	v_shadowcoord = u_shadow_vp * world_pos;
	v_position = (mv * vec4(i_position, 1)).xyz;
	v_normal = normalize(normal * i_normal);
	v_texcoord = vec2(i_texcoord.x, 1 - i_texcoord.y);
	v_color = u_color;

	gl_Position = clip_pos;
}