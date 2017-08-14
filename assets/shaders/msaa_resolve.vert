#version 440 core

layout (location = 0) in vec2 i_position;

out gl_PerVertex
{
	vec4 gl_Position;
};

void main()
{
	gl_Position = vec4(2.0 * i_position - 1.0, 0, 1);
}