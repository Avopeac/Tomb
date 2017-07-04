#version 430

layout (location = 0) in vec2 i_position;
layout (location = 1) in mat4 i_char_instance;

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

	mat4 translate = mat4(1);
	translate[3][0] = i_char_instance[1][0];
	translate[3][1] = i_char_instance[1][1];

	mat4 scale = mat4(1);
	scale[0][0] = i_char_instance[1][2];
	scale[1][1] = i_char_instance[1][3];

	vec4 transformed = translate * scale * vec4(i_position.xy, 0, 1);

	gl_Position = u_viewproj * transformed;

	v_position = transformed.xy;
	
	v_texcoord = 0.5 + 0.5 * i_position.xy;
	v_texcoord.y = 1.0 - v_texcoord.y;
	v_texcoord.x *= (i_char_instance[0][1] - i_char_instance[0][0]) / i_char_instance[2][2];
	v_texcoord.y *= (i_char_instance[0][3] - i_char_instance[0][2]) / i_char_instance[2][3];

	v_texture_array_index = uint(i_char_instance[3][2]);
}