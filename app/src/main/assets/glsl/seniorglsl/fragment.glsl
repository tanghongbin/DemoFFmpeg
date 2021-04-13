#version 300 es                              
precision mediump float;   
in vec3 v_normal;                     
layout(location = 0) out vec4 outColor;
uniform samplerCube s_texture;
void main()                                  
{
   if (gl_FragCoord.x < 500.0) {
      outColor = vec4(1.0,0.0,0.0,1.0);
   } else {
      outColor = vec4(0.0,1.0,0.0,1.0);
   }

}