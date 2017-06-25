#version 430

layout (location = 0) in vec2 i_position;
layout (location = 1) in mat4 i_transform;
layout (location = 5) in uint i_layer;
layout (location = 6) in uint i_animation; 

out gl_PerVertex
{
	vec4 gl_Position;
};

out vec2 v_texcoord;

uniform mat4 u_viewproj;

void main()
{
	gl_Position = u_viewproj * i_transform * vec4(i_position.xy, 0, 1);

	v_texcoord = 0.5 + 0.5 * i_position.xy;
	v_texcoord.y = 1.0 - v_texcoord.y;
}