#version 330 core
out vec4 color;
in vec3 vertex_pos;
in vec3 vertex_normal;
in vec2 vertex_tex;

uniform vec3 campos;
uniform vec3 ldn;

uniform sampler2D tex;
uniform sampler2D tex2; // specular

void main()
{
vec3 lp = vec3(20,20,100); //10,20,100
vec3 dn = normalize(lp-vertex_pos);
vec3 n = normalize(vertex_normal);
float light = dot(dn, n);
if (light < 0)
{
	light = 0;
}
vec4 diffuse = texture(tex,vec2(1. - vertex_tex.x,vertex_tex.y));
color = diffuse * light;
color.a = 1;

vec3 camdir = campos - vertex_pos;
camdir = normalize(camdir);
vec3 r = reflect(vertex_normal, ldn);
float spec = dot(camdir, r);
spec = clamp(spec, 0.0, 1.0); // same as saturate()?
spec = pow(spec, 5); //15

vec4 tspec = texture(tex2, vec2(1. - vertex_tex.x,vertex_tex.y));
color = color + (tspec*spec);
}
