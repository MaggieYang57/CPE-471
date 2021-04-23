#version 330 core

out vec4 color;
in vec3 vertex_pos;

void main()
{
	float distance = distance(vertex_pos, vec3(0, 0, -3));
	color.rgb = vec3(255,255,255);
	color.a = 1;
	if (distance >= 1)
	{
		discard;
	}
}