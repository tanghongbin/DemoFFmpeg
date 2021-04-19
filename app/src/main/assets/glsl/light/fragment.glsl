#version 300 es
precision mediump float;
layout(location = 0) out vec4 outColor;
in vec3 Normal;
in vec3 FragPos;
uniform vec3 lightPos;
uniform vec3 viewPos;


in vec2 texture_vec;
struct Material{
  float shininess;
  sampler2D ambient;
    sampler2D diffuse;
    sampler2D specular;
};
struct Light{
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  vec3 direction;
  float cutOff;
  float outerCutOff;

  float constant;
  float linear;
  float quadratic;
};

struct DirLight{
  vec3 direction;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

struct PointLight {
  vec3 position;

  float constant;
  float linear;
  float quadratic;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform Light light;
uniform Material material;
uniform DirLight dirLight;

// function defination
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{

//  vec3 lightDir = normalize(lightPos - FragPos);
//    vec3 ambient = light.ambient * texture(material.ambient,texture_vec).rgb;
//    vec3 norm = normalize(Normal);
//    // 计算漫反射角度
//    float diff= max(dot(norm,lightDir),0.0);
//    vec3 diffuse = light.diffuse * diff * texture(material.diffuse,texture_vec).rgb;
//    // 计算镜面强度
//    vec3 viewDir = normalize(viewPos - FragPos);
//    vec3 reflectDir = reflect(-lightDir,norm);
//    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
//    vec3 specular =  light.specular * spec  *  vec3(texture(material.specular,texture_vec));
//
//  // 计算聚光
//  float theta = dot(lightDir,normalize(-light.direction));
//  float epsilon = (light.cutOff - light.outerCutOff);
//  float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
//  diffuse  *= intensity;
//  specular *= intensity;
//
//  // 计算衰减
//  float distance = length(lightPos - FragPos);
//  float factor = 1.0 / (light.constant + distance * light.linear + distance * distance * light.quadratic);
//  ambient *= factor;
//  diffuse *= factor;
//  specular *= factor;
//    vec3 result = ambient + diffuse + specular;
//    outColor = vec4(result,1.0);

  // 属性
  vec3 norm = normalize(Normal);
  vec3 viewDir = normalize(viewPos - FragPos);

  // 第一阶段：定向光照
  vec3 result = CalcDirLight(dirLight, norm, viewDir);
  // 第二阶段：点光源
  for(int i = 0; i < NR_POINT_LIGHTS; i++)
  result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
  outColor = vec4(result,1.0);
}


vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
  vec3 lightDir = normalize(-light.direction);
  // 漫反射着色
  float diff = max(dot(normal, lightDir), 0.0);
  // 镜面光着色
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  // 合并结果
  vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, texture_vec));
  vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, texture_vec));
  vec3 specular = light.specular * spec * vec3(texture(material.specular, texture_vec));
  return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
  vec3 lightDir = normalize(light.position - fragPos);
  // 漫反射着色
  float diff = max(dot(normal, lightDir), 0.0);
  // 镜面光着色
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  // 衰减
  float distance    = length(light.position - fragPos);
  float attenuation = 1.0 / (light.constant + light.linear * distance +
  light.quadratic * (distance * distance));
  // 合并结果
  vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, texture_vec));
  vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, texture_vec));
  vec3 specular = light.specular * spec * vec3(texture(material.specular, texture_vec));
  ambient  *= attenuation;
  diffuse  *= attenuation;
  specular *= attenuation;
  return (ambient + diffuse + specular);
}