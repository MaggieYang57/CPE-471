#version 330 core
out vec4 color;
in vec3 vertex_pos;
void main()
{
	//color.rgb = vertex_pos;
	color.rgb = vec3(255,255,255);
	color.a = 0.3;
}
