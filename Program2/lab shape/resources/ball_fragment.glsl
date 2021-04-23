#version 330 core
out vec4 color;
in vec3 vertex_pos;
void main()
{
	float f = length(vertex_pos);
	color.rgb = vec3(1-f*0.5,0,0);
	color.a = 1;
}
