#version 330 core
layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec2 vertTex;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
out vec3 vertex_pos;
out vec3 vertex_normal;
out vec2 vertex_tex;

uniform sampler2D tex;
uniform vec3 camoff;

void main()
{
	//infinite terrain
	vec2 texcoords = vertTex;
	float t = 1./100;
	texcoords -= vec2(camoff.x, camoff.z) * t;
	float height = texture(tex, texcoords/100.).r;
	height *= 30;


	//normal lighting 
	float g = 0.0001; 
	float height1 = texture(tex, texcoords/100. + vec2(0,g)).r;
	height1 *= 30;
	float height2 = texture(tex, texcoords/100. + vec2(g,0)).r;
	height2 *= 30;

	vec3 a,b,c;
	a = vec3(0, height * 50,0);
	b = vec3(1, height1 * 50,0);
	c = vec3(0, height2 * 50,1);
	vertex_normal = -normalize(cross(a-b,a-c));

	//for infinite terrain - offset with camoff
	vec4 tpos = vec4(vertPos, 1.0);
	tpos.z -= camoff.z; 
	tpos.x -= camoff.x;
	tpos.y = -7;
	tpos = M * tpos;

	vertex_pos = tpos.xyz;
	gl_Position = P * V * tpos;
	vertex_tex = vertTex;
}
