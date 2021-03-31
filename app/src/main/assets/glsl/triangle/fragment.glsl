#version 300 es                              
precision mediump float;   
in vec4 v_color;                     
out vec4 fragColor;                          
void main()                                  
{
   vec3 lightColor = vec3(0.0f,0.0f,1.0f);
   vec3 toyColor = vec3(1.0f,0.5f,0.31f);
   fragColor = vec4(toyColor * lightColor,1.0f);
//   fragColor = vec4(0.0,0.0,1.0,1.0f);
}