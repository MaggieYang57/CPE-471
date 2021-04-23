#version 410
#extension GL_ARB_explicit_uniform_location : enable

layout(location = 0) out vec4 color;
layout(location = 1) out vec4 pixelpos;

in vec3 vertex_pos;
in vec3 vertex_normal;
in vec2 vertex_tex;

uniform vec3 campos;
uniform vec3 ldn;

layout(location = 0) uniform sampler2D tex;//color
layout(location = 1) uniform sampler2D tex2;//pos


void main()
{
	vec3 lp = vec3(10,100,100); //10,20,100
	vec3 dn = normalize(lp-vertex_pos);
	

	vec3 n = normalize(vertex_normal);
	float light = dot(dn, n);
	if (light < 0)
	{
		light = 0;
	}
	vec4 diffuse = texture(tex,vec2(1. - vertex_tex.x,vertex_tex.y));
	vec3 pos = texture(tex, vertex_tex).rgb;

	float dist = distance(lp, pos);
	float attenuation = 1.0/(dist);
	light -= attenuation; 

	color = diffuse * light;
	color.a = 1;

	vec3 camdir = campos - vertex_pos;
	camdir = normalize(camdir);
	vec3 r = reflect(vertex_normal, ldn);

	float spec = dot(camdir, r);
	spec = pow(spec, 5); //15
	spec = clamp(spec, 0.0, 1.0); // same as saturate()?


	vec4 tspec = texture(tex2, vec2(1. - vertex_tex.x,vertex_tex.y));

	color += tspec * spec;
	pixelpos.xyz = vertex_pos;
	pixelpos.w = 1;
	color.a=1;
}
