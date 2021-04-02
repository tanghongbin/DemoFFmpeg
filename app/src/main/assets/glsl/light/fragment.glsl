#version 300 es
precision mediump float;
layout(location = 0) out vec4 outColor;
in vec3 Normal;
in vec3 FragPos;
uniform vec3 lightPos;
uniform vec3 viewPos;

uniform sampler2D textSample;
uniform sampler2D borderSample;
uniform sampler2D flowSample;

in vec2 texture_vec;
struct Material{
  float shininess;
};
struct Light{
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};
uniform Light light;
uniform Material material;
void main()
{
  vec3 ambient = light.ambient * texture(textSample,texture_vec).rgb;
  vec3 norm = normalize(Normal);
  vec3 lightDir = normalize(lightPos - FragPos);
  // 计算漫反射角度
  float diff= max(dot(norm,lightDir),0.0);
  vec3 diffuse = light.diffuse * diff * texture(textSample,texture_vec).rgb;
  // 计算镜面强度
  vec3 viewDir = normalize(viewPos - FragPos);
  vec3 reflectDir = reflect(-lightDir,norm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  vec3 specular =  light.specular * spec  * vec3(texture(borderSample,texture_vec));
  vec3 result = ambient + diffuse + specular + vec3(texture(flowSample,texture_vec));
  outColor = vec4(result,1.0);
}