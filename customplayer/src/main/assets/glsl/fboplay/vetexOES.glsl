#version 300 es                            
layout(location = 0) in vec4 a_position;
layout(location = 1) in vec2 in_texCoords;

out vec2 out_texCoords;
uniform mat4 model;
uniform mat4 uTextureMatrix;
void main()
{
   out_texCoords = (uTextureMatrix * vec4(in_texCoords,0.0f,1.0f)).xy;
   gl_Position = model * a_position;
}