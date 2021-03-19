#version 300 es
layout(location = 0) in vec4 a_position;
layout(location = 1) in vec3 a_normal;
out vec3 v_normal;
out float outfactor;
in float infactor;
void main()                              
{                         
   v_normal = a_normal;
   gl_Position = a_position;
   outfactor = infactor;
}