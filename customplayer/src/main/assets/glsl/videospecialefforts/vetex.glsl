#version 300 es                            
layout(location = 0) in vec4 a_position;
layout(location = 1) in vec2 in_texCoords;
uniform mat4 mMvpMatrix;
uniform int v_type;
out vec2 out_texCoords;

void main()
{
   if (v_type == 2) {
      gl_Position = mMvpMatrix * a_position;
   } else {
      gl_Position = a_position;
   }

   out_texCoords = in_texCoords;
}