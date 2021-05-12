#version 300 es
precision mediump float;                            
in vec2 v_texCoord;                                 
layout(location = 0) out vec4 outColor;             
uniform sampler2D s_TextureMap;

float near = 0.1;
float far  = 100.0;

float LinearizeDepth(float depth)
{
  float z = depth * 2.0 - 1.0; // back to NDC
  return (2.0 * near * far) / (far + near - z * (far - near));
}

void main()                                         
{
  float depth = LinearizeDepth(gl_FragCoord.z) / far; // 为了演示除以 far
  outColor = texture(s_TextureMap, v_texCoord);
//  outColor = vec4(vec3(depth),1.0);
}