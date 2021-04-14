#version 300 es                            
layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in mat4 model;

out vec2 outTexCoords;
out vec3 Normal;
uniform mat4 view;
uniform mat4 projection;

void main()
{
   outTexCoords = aTexCoords;
   gl_Position = projection *view * model * vPosition;
   Normal = mat3(transpose(inverse(model))) * aNormal;
}