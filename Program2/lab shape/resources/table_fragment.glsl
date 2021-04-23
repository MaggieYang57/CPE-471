#version 330 core
out vec4 color;
in vec3 vertex_pos;
void main()
{
	color.rgb = vec3(.5,.9,.6);
	color.a = 1;
}
