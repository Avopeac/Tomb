#version 430

layout (location = 0) in vec2 i_position;
layout (location = 1) in mat4 i_transform;
layout (location = 5) in uint i_texture_array_index;

out gl_PerVertex
{
	vec4 gl_Position;
};

out vec2 v_texcoord;

out vec2 v_position;

flat out uint v_texture_array_index;

uniform mat4 u_viewproj;

uniform float u_time;

void main()
{

	mat4 transform = mat4(1);
	transform[3][0] = i_transform[1][0];
	transform[3][1] = i_transform[1][1];
	transform[0][0] = i_transform[1][2];
	transform[1][1] = i_transform[1][3];

	vec4 transformed = transform * vec4(i_position.xy, 0, 1);

	gl_Position = u_viewproj * transformed;

	v_position = transformed.xy;
	v_texcoord = 0.5 + 0.5 * i_position.xy;
	v_texcoord.y = 1.0 - v_texcoord.y;
	v_texture_array_index = i_texture_array_index;
}