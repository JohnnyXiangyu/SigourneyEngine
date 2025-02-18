#version 330 core

//in vec4 v_VertColor;

layout(location = 0) out vec4 color;

void main()
{
	// use the input vertex color
	color = vec4(1.0, 0.0, 0.0, 1.0);
};
