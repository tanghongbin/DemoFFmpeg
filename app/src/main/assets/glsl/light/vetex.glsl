#version 300 es                            
layout(location = 0) in vec4 a_position;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 inVec_text;
uniform mat4 model;
uniform mat4 mMvpMatrix;
out vec3 FragPos;
out vec3 Normal;
out vec2 texture_vec;
void main()
{
   gl_Position = mMvpMatrix * a_position;
   FragPos = vec3(model * a_position);
   Normal = mat3(transpose(inverse(model))) * aNormal;
   texture_vec = inVec_text;
}