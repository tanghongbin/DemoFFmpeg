#version 300 es
precision mediump float;
layout(location = 0) out vec4 outColor;
in vec3 Normal;
in vec3 FragPos;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
void main()
{
  float ambientStrength = 0.1f;
  vec3 ambient = lightColor * ambientStrength;
  vec3 norm = normalize(Normal);
  vec3 lightDir = normalize(lightPos - FragPos);
//    vec3 result = (ambient) * objectColor;
  // 计算漫反射角度
  float diff= max(dot(norm,lightDir),0.0);
  vec3 diffuse = diff * lightColor;
//  vec3 result = (ambient + diffuse) * objectColor;
  // 计算镜面强度
  float specularStrength = 0.5;
  vec3 viewDir = normalize(viewPos - FragPos);
  vec3 reflectDir = reflect(-lightDir,norm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
  vec3 specular = specularStrength * spec * lightColor;
  vec3 result = (ambient + diffuse + specular) * objectColor;
  outColor = vec4(result,1.0);
}