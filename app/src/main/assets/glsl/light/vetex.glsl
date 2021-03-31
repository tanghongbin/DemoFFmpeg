#version 300 es                            
layout(location = 0) in vec4 a_position;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main()
{
   gl_Position = projection * view * model * a_position;
}