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

uniform mat4 u_vp;

void main()
{
	gl_Position = u_vp * world_transforms[gl_InstanceID] * vec4(i_position, 1);
}