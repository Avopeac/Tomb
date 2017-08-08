#version 410

layout (location = 0) in vec2 i_position;

out gl_PerVertex
{
	vec4 gl_Position;
};

out vec2 v_texcoord;

void main()
{

	gl_Position = u_viewproj * vec4(i_position, 0, 1);
	v_texcoord = 0.5 + 0.5 * i_position.xy;
	v_texcoord.y = 1.0 - v_texcoord.y;
}