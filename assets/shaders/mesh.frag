#version 440 core

in vec3 v_direction;
in vec2 v_texcoord;
in flat vec3 f_sun;
in flat vec3 f_normal;
in flat vec4 f_color;

out vec4 o_color;

void main()
{
	float intensity = 0.0;
	float ndotl = clamp(dot(f_normal, -f_sun), 0.0, 1.0);
	if (ndotl > 0.0) {

		vec3 h = normalize(v_direction - f_sun);
		float ndoth = clamp(dot(f_normal, h), 0.0, 1.0);

		intensity = pow(ndoth, 80.0);
	}


	vec3 ambient = f_color.rgb * 0.1;

	vec3 specular_color = vec3(1,1,1);

	o_color = vec4(ambient + ndotl * f_color.rgb + intensity * specular_color, f_color.a);
}