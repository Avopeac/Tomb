#version 440 core

layout(location = 0) out vec3 o_color;

in vec2 v_texcoord;
in vec3 v_sun;

uniform float u_time;

uniform sampler2D u_albedo;
uniform sampler2D u_position;
uniform sampler2D u_normal;
uniform sampler2D u_depth;

void main()
{
	vec3 albedo = texture(u_albedo, v_texcoord).rgb; 
  vec3 normal = texture(u_normal, v_texcoord).rgb; 
  vec3 direction = -normalize(texture(u_position, v_texcoord).rgb);

  float ndotl = max(0.0, dot(normal, v_sun));

  vec3 half_vec = normalize(direction + v_sun);
  float ndoth = max(0.0, dot(normal, half_vec));
  float specularity = pow(ndoth, 70.0);

	o_color = albedo * ndotl; //+ vec3(specularity);
}