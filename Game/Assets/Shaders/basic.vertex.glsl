#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

out vec3 v_Normal;
out vec2 v_Uv;

uniform mat4 u_MVP;

void main()
{
	gl_Position = u_MVP * vec4(position, 1.0);

	// forward the attributes into fragment shader 
	v_Normal = normal;
	v_Uv = uv;
};
