#version 330 core
out vec4 color;
in vec3 vertex_pos;
void main()
{
	//color = vec4(.678,.847,.902,1);
	color.rgb = vertex_pos;
	color.a = 1;
}
