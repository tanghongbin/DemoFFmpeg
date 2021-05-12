#version 300 es
layout(location = 0) in vec4 a_position;
layout(location = 1) in vec2 a_texCoords;
layout(location = 2) in vec3 a_normal;

uniform mat4 mvp;

void main()                              
{
   gl_Position = mvp * a_position;
}