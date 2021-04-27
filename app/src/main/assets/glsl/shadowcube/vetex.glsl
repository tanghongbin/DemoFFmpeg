#version 300 es
layout(location = 0) in vec4 a_position;
layout(location = 1) in vec2 a_texCoords;
layout(location = 2) in vec3 a_normal;


out vec3 v_normal;
out vec2 TexCoords;
out vec3 FragPos;
out vec4 FragPosLightSpace;


uniform mat4 mvp;
uniform mat4 model;
uniform int isNormalTexture;
uniform mat4 lightSpaceMatrix;

void main()                              
{
   gl_Position = mvp * a_position;
   TexCoords = a_texCoords;
   v_normal = mat3(transpose(inverse(model))) * a_normal;
   FragPos = vec3(model * a_position);
   FragPosLightSpace = lightSpaceMatrix * (vec4(FragPos,1.0));
}