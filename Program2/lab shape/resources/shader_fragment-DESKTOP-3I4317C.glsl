#version 330 core
out vec4 color;
in vec3 vertex_pos;
void main()
{
	color.rgb = vertex_pos;
	color.a = 1;
}
