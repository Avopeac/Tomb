#version 440 core

layout(location = 0) out vec3 o_color;

in vec2 v_texcoord;
in vec3 v_sun;

uniform float u_time;

uniform sampler2D u_albedo;
uniform sampler2D u_position;
uniform sampler2D u_normal;
uniform sampler2D u_shadow;
uniform sampler2D u_depth;
uniform samplerCube u_skybox;

uniform mat4 u_view;
uniform mat4 u_inv_proj;
uniform mat4 u_inv_view;

vec3 getWorldDirection(vec2 texcoord) {

  vec2 ndc = 2.0 * texcoord - 1.0;
  vec4 view = u_inv_proj * vec4(ndc, 1, 1);
  view /= view.w;
  vec4 world = u_inv_view * view;
  world /= world.w;
  return normalize(vec4(world.xyz, 0.0));
}

float eyeZ(float z, float n, float f) {
  return (2 * n * f) / (f + n - z * (f - n));
}

float fog(vec3 color, float depth) {
  if (depth > 0.9999) {
    o_color = vec3(0.4, 0.5, 0.9);
  }
  
  vec3 fog_col = vec3(0.4, 0.5, 0.6);
  float fog_strength = 0.1;
  float fog_amount = 1.0 - exp(-eyeZ(depth, 0.01, 100.0) * fog_strength);
  o_color = mix(o_color, fog_col, fog_amount);
  return o_color;
}

void main()
{
  vec3 sun = normalize(v_sun);

	vec3 albedo = texture(u_albedo, v_texcoord).rgb; 
  vec3 normal = normalize(texture(u_normal, v_texcoord).rgb);
  vec3 position = texture(u_position, v_texcoord).rgb;
  float shadow = texture(u_shadow, v_texcoord).r;
  float depth = texture(u_depth, v_texcoord).r;

  float ndotl = max(0.0, dot(normal, sun));

  vec3 direction = -normalize(position);
  vec3 half_vec = normalize(direction + sun);
  float ndoth = max(0.0, dot(normal, half_vec));
  float specularity = pow(ndoth, 16.0);

  vec3 ambient = 0.1 * albedo;

	o_color = texture(u_skybox, getWorldDirection(v_texcoord)).rgb;
  if (depth <= 0.9999) {
    o_color = ambient + clamp(shadow, 0.5, 1.0) * (albedo * ndotl + vec3(specularity));
  }
}