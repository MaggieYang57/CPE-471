#version 330 core
out vec4 color;
in vec3 vertex_pos;
void main()
{
	color.rgb = vec3(1,0,0);
	color.a = 1;
}
