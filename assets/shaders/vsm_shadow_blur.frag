#version 440 core

layout(location = 0) out vec2 o_vsm_depth;

in vec2 v_texcoord;

uniform sampler2D u_vsm_shadow;
uniform int u_direction;
uniform vec2 u_texel_size;

void main()
{
	vec2 vsm_depth = texture(u_vsm_shadow, v_texcoord).rg; 

	float hstep = u_texel_size.x * float(u_direction);
	float vstep = u_texel_size.y * float(1.0 - u_direction);

	vec2 sum = vec2(0.0);
	sum += texture(u_vsm_shadow, vec2(v_texcoord.x - 4.0 * hstep, v_texcoord.y - 4.0 * vstep)).rg * 0.0162162162;
	sum += texture(u_vsm_shadow, vec2(v_texcoord.x - 3.0 * hstep, v_texcoord.y - 3.0 * vstep)).rg * 0.0540540541;
	sum += texture(u_vsm_shadow, vec2(v_texcoord.x - 2.0 * hstep, v_texcoord.y - 2.0 * vstep)).rg * 0.1216216216;
	sum += texture(u_vsm_shadow, vec2(v_texcoord.x - 1.0 * hstep, v_texcoord.y - 1.0 * vstep)).rg * 0.1945945946;
	sum += texture(u_vsm_shadow, vec2(v_texcoord.x, v_texcoord.y)).rg * 0.2270270270;
	sum += texture(u_vsm_shadow, vec2(v_texcoord.x + 1.0 * hstep, v_texcoord.y + 1.0 * vstep)).rg * 0.1945945946;
	sum += texture(u_vsm_shadow, vec2(v_texcoord.x + 2.0 * hstep, v_texcoord.y + 2.0 * vstep)).rg * 0.1216216216;
	sum += texture(u_vsm_shadow, vec2(v_texcoord.x + 3.0 * hstep, v_texcoord.y + 3.0 * vstep)).rg * 0.0540540541;
	sum += texture(u_vsm_shadow, vec2(v_texcoord.x + 4.0 * hstep, v_texcoord.y + 4.0 * vstep)).rg * 0.0162162162;

	o_vsm_depth = sum;
}