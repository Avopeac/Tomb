#version 440 core

in flat vec3 v_normal;
in vec2 v_texcoord;
in flat vec3 f_sun;

out vec4 o_color;

void main()
{
	float ndotl = clamp(dot(normalize(v_normal), -f_sun), 0, 1);

	vec3 color = vec3(1, 1, 0);

	vec3 ambient = color * 0.1;

	o_color = vec4(ambient + ndotl * color, 1);
}