#version 330 core
out vec4 color;

in vec3 vertex_pos;
in vec2 vertex_tex;
in vec3 vertex_normal;

uniform sampler2D tex;
uniform sampler2D tex2;
uniform vec3 camoff;
uniform vec3 campos;

void main()
{
	vec2 texcoords = vertex_tex;
	float t = 1./100;
	texcoords -= vec2(camoff.x, camoff.z) * t;

	vec3 texturecolor = texture(tex2,texcoords*50).rgb;
	texturecolor.r += pow(texturecolor.r, 3);
	color.rgb = texturecolor;
	color.a = 1;

	//faded background
	float len = length(vertex_pos.xz + campos.xz);
	len -= 45;
	len /= 10.;
	len = clamp(len, 0, 1);
	color.a = 1 - len;

	//normal lighting
	vec3 lp = vec3(100,500,0);
	vec3 ld = normalize(lp-vertex_pos);
	vec3 n = normalize(vertex_normal);
	float light = dot(ld, n);
	light = clamp(light, 0.1, 1);

	//ambient
	color.rgb = texturecolor * light;
	color.r += 0.3;
	color.g += 0.1;
	color.b += 0.1;

	//specular
	vec3 camdir = normalize(campos - vertex_pos);
	vec3 halfVec = normalize(camdir + ld);
	float spec = dot(n, halfVec);
	spec = clamp(spec, 0, 1);
	spec = pow(spec, 100);

	color.rbg += spec;
}
