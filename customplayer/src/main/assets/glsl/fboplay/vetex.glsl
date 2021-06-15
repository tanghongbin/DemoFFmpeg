#version 300 es                            
layout(location = 0) in vec4 a_position;
layout(location = 1) in vec2 in_texCoords;

out vec2 out_texCoords;
uniform mat4 model;
void main()
{
   gl_Position = model * a_position;
   out_texCoords = in_texCoords;
}