#version 410

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

#define FONT_TEXTURE_SIZE 128

#define FONT_TEXTURE_INV_SIZE 0.0078125 // 1.0 / 128.0

void main()
{


	float x_scale = i_char_instance[3][2] - i_char_instance[0][0];
	float y_scale = i_char_instance[3][3]; 

	float x_offset = i_char_instance[1][0] + i_char_instance[3][0] * 2.0;
	float y_offset = i_char_instance[1][1];

	vec4 transformed = vec4(i_position.xy, 0, 1);
	transformed.x = transformed.x * x_scale + x_offset;
	transformed.y = transformed.y * y_scale + y_offset;
	gl_Position = u_viewproj * transformed;

	v_position = transformed.xy;
	v_texcoord = 0.5 + 0.5 * i_position.xy;
	v_texcoord.y = 1.0 - v_texcoord.y;
	v_texcoord.x *= i_char_instance[3][2] * FONT_TEXTURE_INV_SIZE;
	v_texcoord.y *= i_char_instance[3][3] * FONT_TEXTURE_INV_SIZE;
	v_texture_array_index = uint(i_char_instance[3][1]);
}