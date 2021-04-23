#version 330 core
layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec2 vertTex;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
out vec3 vertex_pos;
out vec2 vertex_tex;
out vec3 vertex_normal;

uniform sampler2D tex;
uniform vec3 camoff; 

float hashHeight(float n)
{ 
	return fract(sin(n) * 753.5453123); 
}

float snoise(vec3 x)
{
	vec3 p = floor(x); 
	vec3 f = fract(x);
	f = f * f *(3.0 - 2.0 * f);
	float n = p.x + p.y * 157.0 + 113.0 * p.z;
	return mix(mix(mix(hashHeight(n + 0.0), hashHeight(n + 1.0), f.x),
		mix(hashHeight(n + 157.0), hashHeight(n + 158.0), f.x), f.y), mix(mix(hashHeight(n + 113.0),
			hashHeight(n + 114.0), f.x), mix(hashHeight(n + 270.0), hashHeight(n + 271.0), f.x), f.y), f.z);
}

float noise(vec3 position, int octaves, float frequency, float persistence)
{
	float total = 0.0;
	float maxAmplitude = 0.0;
	float amplitude = 1.0;
	for (int i = 0; i < octaves; i++) 
	{ 
		total += snoise(position * frequency) * amplitude; frequency *= 2.0;
		maxAmplitude += amplitude; 
		amplitude *= persistence; 
	}
	return total / maxAmplitude;
}

void main()
{
	//infinite terrain
	vec2 texcoords = vertTex;
	float t = 1./100;
	texcoords -= vec2(camoff.x, camoff.z) * t;
	float height = texture(tex, texcoords/100.).r;
	height *= 100;
	height -= 12;

	//normal lighting 
	float g = 0.0001; 
	float height1 = texture(tex, texcoords/100. + vec2(0,g)).r;
	height1 *= 100;
	height1 -= 12;
	float height2 = texture(tex, texcoords/100. + vec2(g,0)).r;
	height2 *= 100;
	height2 -= 12;

	vec3 a,b,c;
	a = vec3(0, height * 50,0);
	b = vec3(1, height1 * 50,0);
	c = vec3(0, height2 * 50,1);
	vertex_normal = -normalize(cross(a-b,a-c));

	//for infinite terrain - offset with camoff
	vec4 tpos = vec4(vertPos, 1.0);
	tpos.z -= camoff.z; 
	tpos.x -= camoff.x;
	tpos = M * tpos;

	float procheight = noise(tpos.xzy, 9, 0.03, 0.6);
	float baseheight = noise(tpos.xzy, 4, 0.004, 0.3);
	baseheight = pow(baseheight, 3);
	procheight = baseheight*procheight;
	procheight *= 50;

	tpos.y += height + procheight;
	vertex_pos = tpos.xyz;

	gl_Position = P * V * tpos;
	vertex_tex = vertTex;
}
