#version 300 es
layout(location = 0) in vec3 a_position;
out vec3 TexCoords;
uniform mat4 u_MVPMatrix;
void main()
{
   TexCoords = a_position;
   vec4 pos = u_MVPMatrix * vec4(a_position, 1.0);
   gl_Position = pos.xyzw;
}