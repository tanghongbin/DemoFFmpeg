#version 300 es
uniform mat4 m_MVPMatrix;
layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec4 a_color;
out vec4 v_color; 
void main()                              
{                         
   v_color = a_color;            
   gl_Position = vPosition;
}