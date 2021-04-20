#version 300 es                              
precision mediump float;   
in vec3 v_normal;
in vec2 TexCoords;
in vec3 FragPos;
layout(location = 0) out vec4 outColor;

struct PointLight {
   vec3 position;

   float constant;
   float linear;
   float quadratic;

   vec3 ambient;
   vec3 diffuse;
   vec3 specular;
   vec3 lightColor;
};

struct Material{
   float shininess;
   sampler2D ambient;
   sampler2D diffuse;
   sampler2D specular;
};

uniform Material material;
uniform PointLight pointLight;
uniform vec3 viewPos;
// 1-是地板
uniform int type;

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()                                  
{
   // 属性
//   if(type == 1){
//      outColor = texture(material.diffuse,TexCoords);
//   } else {
      vec3 norm = normalize(v_normal);
      vec3 viewDir = normalize(viewPos - FragPos);
      vec3 result = CalcPointLight(pointLight, norm, FragPos, viewDir);
      outColor = vec4(result,1.0);
//   }

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
//   float attenuation = 1.0 / (light.constant + light.linear * distance +
//   light.quadratic * (distance * distance));
   // 暂时不考虑衰减
   float attenuation = 1.0;

   // 合并结果
   vec3 ambient  = light.lightColor * light.ambient  * vec3(texture(material.diffuse, TexCoords));
   vec3 diffuse  = light.lightColor * light.diffuse  * diff * vec3(texture(material.diffuse, TexCoords));
   vec3 specular = light.lightColor * light.specular * spec * vec3(texture(material.specular, TexCoords));
   ambient  *= attenuation;
   diffuse  *= attenuation;
   specular *= attenuation;
   return (ambient + diffuse + specular);
}