#version 300 es
layout(location = 0) in vec4 a_position;
layout(location = 1) in vec2 a_texCoords;
layout(location = 2) in vec3 a_normal;


out vec3 v_normal;
out vec2 TexCoords;
out vec3 FragPos;
uniform mat4 mvp;
uniform mat4 model;

void main()                              
{

   TexCoords = a_texCoords;
   gl_Position = mvp * a_position;
   FragPos = vec3(model * a_position);
   v_normal = mat3(transpose(inverse(model))) * a_normal;

}