#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 vertColor;

out vec4 v_VertColor;

uniform mat4 u_MVP;

void main()
{
   gl_Position = u_MVP * position;

   // the color is directly copied to the fragment shader;
   // TODO: need to add a shading algorithm
   v_VertColor = vertColor;
};
