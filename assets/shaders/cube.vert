#version 440 core

layout (location = 0) in vec3 i_position;

out gl_PerVertex
{
	vec4 gl_Position;
};

uniform mat4 u_viewproj;

void main()
{
	gl_Position = u_viewproj * vec4(i_position, 1);


	//gl_Position = vec4(i_position, 1);

}