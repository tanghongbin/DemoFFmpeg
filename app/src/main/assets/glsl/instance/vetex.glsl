#version 300 es                            
layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec2 outTexCoords;
out vec3 Normal;
uniform mat4 model;

void main()
{
   outTexCoords = aTexCoords;
   gl_Position = m_MVPMatrix * vPosition;
   Normal = mat3(transpose(inverse(model))) * aNormal;
}