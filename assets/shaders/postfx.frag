#version 440 core


//
// Description : Array and textureless GLSL 2D simplex noise function.
//      Author : Ian McEwan, Ashima Arts.
//  Maintainer : stegu
//     Lastmod : 20110822 (ijm)
//     License : Copyright (C) 2011 Ashima Arts. All rights reserved.
//               Distributed under the MIT License. See LICENSE file.
//               https://github.com/ashima/webgl-noise
//               https://github.com/stegu/webgl-noise
// 

vec3 mod289(vec3 x)
{
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec2 mod289(vec2 x)
{
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec3 permute(vec3 x) 
{
  return mod289(((x*34.0)+1.0)*x);
}

float snoise(vec2 v)
{
  const vec4 C = vec4(0.211324865405187,  // (3.0-sqrt(3.0))/6.0
                      0.366025403784439,  // 0.5*(sqrt(3.0)-1.0)
                     -0.577350269189626,  // -1.0 + 2.0 * C.x
                      0.024390243902439); // 1.0 / 41.0
  // First corner
  vec2 i  = floor(v + dot(v, C.yy) );
  vec2 x0 = v -   i + dot(i, C.xx);

  // Other corners
  vec2 i1;
  //i1.x = step( x0.y, x0.x ); // x0.x > x0.y ? 1.0 : 0.0
  //i1.y = 1.0 - i1.x;
  i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
  // x0 = x0 - 0.0 + 0.0 * C.xx ;
  // x1 = x0 - i1 + 1.0 * C.xx ;
  // x2 = x0 - 1.0 + 2.0 * C.xx ;
  vec4 x12 = x0.xyxy + C.xxzz;
  x12.xy -= i1;

  // Permutations
  i = mod289(i); // Avoid truncation effects in permutation
  vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 ))
		+ i.x + vec3(0.0, i1.x, 1.0 ));

  vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy), dot(x12.zw,x12.zw)), 0.0);
  m = m*m ;
  m = m*m ;

  // Gradients: 41 points uniformly over a line, mapped onto a diamond.
  // The ring size 17*17 = 289 is close to a multiple of 41 (41*7 = 287)

  vec3 x = 2.0 * fract(p * C.www) - 1.0;
  vec3 h = abs(x) - 0.5;
  vec3 ox = floor(x + 0.5);
  vec3 a0 = x - ox;

  // Normalise gradients implicitly by scaling m
  // Approximation of: m *= inversesqrt( a0*a0 + h*h );
  m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );

  // Compute final noise value at P
  vec3 g;
  g.x  = a0.x  * x0.x  + h.x  * x0.y;
  g.yz = a0.yz * x12.xz + h.yz * x12.yw;
  return 130.0 * dot(m, g);
}

vec3 water(vec2 uv, float t) 
{
	float noise = 0.5 + 0.5 * snoise(vec2(20.0 * uv.x + 0.1 * t, 20.0 * uv.y));
	vec3 col0 = vec3(0.1, 0.5, 0.6);
	vec3 col1 = vec3(0.2, 0.7, 0.9);
	return mix(col0, col1, noise);
}

vec3 clouds(vec3 bg, vec2 uv, float t)
{
	float noise = 0.5 + 0.5 * snoise(vec2(0.01 * t + 4.0 * uv.x, 4.0 * uv.y));
	vec3 col0 = vec3(1.0, 1.0, 1.0);
	vec3 col1 = vec3(0.4, 0.4, 0.4);
	return mix(bg, col1, noise);
}

vec3 reinhardt_tonemap(vec3 hdr)
{
	return hdr; //hdr / (1 + hdr);
}

vec3 gamma_correction(vec3 ldr)
{
	return pow(ldr, 1.0 / vec3(2.2));
}

layout(location = 0) out vec4 o_color;

in vec2 v_texcoord;

uniform float u_time;
uniform sampler2D u_texture0;
uniform sampler2D u_texture1;

void main()
{
	vec4 tex_col = texture(u_texture0, v_texcoord); 

	//float tex_depth = texture(u_texture1, v_texcoord).r;
	//if (tex_depth > 0.9)
	//{
		//tex_col.rgb = water(v_texcoord, u_time);
	//}


	//if (abs(dFdx(tex_depth)) > 0.1 || abs(dFdy(tex_depth)) > 0.1) {
		//tex_col.rgb = vec3(1);
	//}


	//tex_col.rgb += clouds(tex_col.rgb, v_texcoord, u_time);

	//o_color.rgb = reinhardt_tonemap(tex_col.rgb);
	//o_color.rgb = gamma_correction(o_color.rgb);
	o_color.rgb = tex_col.rgb;
	o_color.a = tex_col.a;
}