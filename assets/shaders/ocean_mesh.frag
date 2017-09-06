#version 440 core

in vec3 v_direction;
in vec2 v_texcoord;
in flat vec3 f_sun;
in vec3 f_normal;
in vec4 f_color;

out vec4 o_color;

void main()
{

	vec3 sun_dir = -f_sun;

	float sun = clamp(dot(f_normal, sun_dir), 0.0, 1.0);
    float sky = clamp(0.5 + 0.5 * f_normal.y, 0.0, 1.0);
    float ind = clamp(dot(f_normal, normalize(sun_dir * vec3(-1.0, 0.0, -1.0))), 0.0, 1.0);

    // compute lighting
    vec3 lin  = sun * vec3(1.64,1.27,0.99);
         lin += sky * vec3(0.16,0.20,0.28);
         lin += ind * vec3(0.40,0.28,0.20);

	float specularity = 0.0;
	if (sun > 0.0) {
		vec3 half_vec = normalize(v_direction + sun_dir);
		float ndoth = clamp(dot(f_normal, half_vec), 0.0, 1.0);
		specularity = pow(ndoth, 80.0);
	}

	o_color = vec4(lin * (f_color.rgb + vec3(specularity)), f_color.a);
}