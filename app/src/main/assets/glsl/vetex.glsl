#version 300 es                            
layout(location = 0) in vec4 a_position;   
layout(location = 1) in vec2 a_texCoord;   
layout(location = 2) in float inFactor;
out float outFactor;
out vec2 v_texCoord;                       
void main()                                
{                   
   gl_Position = a_position;               
   v_texCoord = a_texCoord;   
   outFactor = inFactor;
}