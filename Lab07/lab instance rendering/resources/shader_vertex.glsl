#version 410 core
layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in vec2 vertTex;
layout (location = 3) in vec4 InstancePos; // not going to first vertex , first element goes to ALL vertices, repeat for second instance and second element

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform float time;

out vec3 vertex_pos;
out vec3 vertex_normal;
out vec2 vertex_tex;
void main()
{
	vertex_normal = vec4(M * vec4(vertNor,0.0)).xyz;
	vec4 pos = M * vec4(vertPos,1.0) +InstancePos;
	
	float dist = distance(InstancePos, vec4(0.0, 0.0, 0.0, 1) );
	float angle = time * dist * 5;

	mat4 RotateY = mat4(
    vec4( cos(angle), 0.0,sin(angle), 0.0),
    vec4( 0.0, 1., 0.0, 0.0),
    vec4( -sin(angle), 0.0, cos(angle), 0.0),
    vec4( 0.0, 0.0, 0.0, 1.0) );

	vec4 tempinstancepos = RotateY * InstancePos;
	
	gl_Position = P * V * (tempinstancepos + pos);
	vertex_tex = vertTex;	
}
