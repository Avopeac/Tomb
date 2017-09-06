#version 440 core

layout (location = 0) in vec3 i_position;
layout (location = 1) in vec3 i_normal;
layout (location = 2) in vec2 i_texcoord;

out gl_PerVertex
{
	vec4 gl_Position;
};

out vec2 v_texcoord;
out vec3 v_direction;
out vec3 f_normal;
out flat vec3 f_sun;
out vec4 f_color;

uniform mat4 u_mvp;
uniform mat4 u_view;
uniform mat4 u_vp;
uniform mat4 u_normal;
uniform vec4 u_color;
uniform float u_time;

void main()
{

	vec3 wave_pos = i_position;

	vec2 d0 = normalize(vec2(-1,1));
	vec2 d1 = normalize(vec2(0,-1));
	vec2 d2 = normalize(vec2(1,0));
	vec2 d3 = normalize(vec2(-1,0));

	vec4 w = vec4(22.0, 20.0, 34.0, 18.0);
	vec4 a = vec4(0.002, 0.003, 0.001, 0.009);
	vec4 p = vec4(200.0, 100.5, 100.9, 100.4);

	vec4 cd = vec4(cos(dot(d0, wave_pos.xy) * w[0] + u_time * p[0]),
				   cos(dot(d1, wave_pos.xy) * w[1] + u_time * p[1]),
				   cos(dot(d2, wave_pos.xy) * w[2] + u_time * p[2]),
				   cos(dot(d3, wave_pos.xy) * w[3] + u_time * p[3]));

	float h = a[0] * sin(dot(d0, wave_pos.xy) * w[0] + u_time * p[0]) +
			  a[1] * sin(dot(d1, wave_pos.xy) * w[1] + u_time * p[1]) +
		      a[2] * sin(dot(d2, wave_pos.xy) * w[2] + u_time * p[2]) +
			  a[3] * sin(dot(d3, wave_pos.xy) * w[3] + u_time * p[3]);

	float hdx = w[0] * d0[0] * a[0] * cd[0] +
				w[1] * d1[0] * a[1] * cd[1] + 
				w[2] * d2[0] * a[2] * cd[2] + 
				w[3] * d3[0] * a[3] * cd[3]; 

	float hdy = w[0] * d0[1] * a[0] * cd[0] +
				w[1] * d1[1] * a[1] * cd[1] + 
				w[2] * d2[1] * a[2] * cd[2] + 
				w[3] * d3[1] * a[3] * cd[3];

	vec3 n = normalize(vec3(-hdx, -hdy, 1));

	wave_pos.z = h;

	gl_Position = u_mvp * vec4(wave_pos, 1);

	v_direction = normalize((u_view * vec4(wave_pos, 1)).xyz);
	v_texcoord = i_texcoord;
	f_color = u_color;
	f_sun = normalize((u_view * vec4(0, 1, -0.5, 0.0)).xyz);
	f_normal = normalize((u_normal * vec4(n, 0)).xyz);
}