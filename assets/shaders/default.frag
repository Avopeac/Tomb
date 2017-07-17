#version 410

out vec4 o_color;

in vec2 v_texcoord;

in vec2 v_position;

uniform sampler2D u_texture;

uniform float u_time;

uniform vec2 u_mouse;

void main()
{
	vec2 texcoord_wobble = 0.005 * vec2(0.0, sin(5.0 * u_time + v_position.y));
	o_color = texture(u_texture, v_texcoord + texcoord_wobble);


	//if (length(v_position - u_mouse) < 0.1)
	//if (u_mouse.x > 0.5)
		//o_color.rgb = vec3(1,0,1);

	o_color.rg = v_position / 1024.0;


	o_color.b = 0;

}