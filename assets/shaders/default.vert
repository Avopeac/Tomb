#version 430

out gl_PerVertex
{
	vec4 gl_Position;
}

void main()
{
	gl_Position = vec4(0, 0, 1, 1);
}