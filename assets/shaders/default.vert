#version 430

in vec2 inPosition;

out gl_PerVertex
{
	vec4 gl_Position;
};

void main()
{
	gl_Position = vec4(inPosition.xy, 0, 1);
}