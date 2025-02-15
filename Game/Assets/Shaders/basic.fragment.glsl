#version 330 core

in vec4 v_VertColor;

layout(location = 0) out vec4 color;

uniform sampler2D u_Texture;

void main()
{
	// use the input vertex color
	color = v_VertColor;
};
