#version 300 es
layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 inNormal;

out vec3 Normal;
out vec3 Position;

uniform mat4 u_Model;
uniform mat4 u_MVPMatrix;
void main()
{
   Normal = mat3(transpose(inverse(u_Model))) * inNormal;;
   vec4 tempPos = u_MVPMatrix * vec4(a_position, 1.0);
   Position = vec3(u_Model * vec4(a_position, 1.0));
   gl_Position = tempPos;
}