#version 300 es
precision mediump float;
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

in vec3 v_normal;
in vec2 TexCoords;
in vec3 FragPos;
in vec4 FragPosLightSpace;

uniform Material material;
uniform PointLight pointLight;
uniform vec3 viewPos;
// 1-是地板,2-是深度渲染
uniform int type;
uniform sampler2D shadowMap;

/**点光源计算***/
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 FragPos, vec3 viewDir);

/**阴影计算***/
float ShadowCalculation(vec4 FragPosLightSpace);

// 参照demo的计算阴影的全部代码
vec4 CalShadowAll();

void main()                                  
{
   // 属性
   vec3 normal = normalize(v_normal);
   vec3 viewDir = normalize(viewPos - FragPos);
//   vec3 result = CalcPointLight(pointLight, norm, FragPos, viewDir);
   // 显示全部过程
   vec3 lightDir = normalize(vec3(-3.0,3.0,-2.0));
   // 漫反射着色
   float diff = max(dot(normal, lightDir), 0.0);
   // 镜面光着色
   vec3 reflectDir = reflect(-lightDir, normal);
   float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
   // 衰减
   float distance    = length(pointLight.position - FragPos);
   //   float attenuation = 1.0 / (pointLight.constant + pointLight.linear * distance +
   //   pointLight.quadratic * (distance * distance));
   // 暂时不考虑衰减
   float attenuation = 1.0;

   // 合并结果
   vec3 color = vec3(texture(material.diffuse, TexCoords));
   vec3 ambient  = pointLight.lightColor * pointLight.ambient ;
   vec3 diffuse  = pointLight.lightColor * pointLight.diffuse  * diff ;
   vec3 specular = pointLight.lightColor * pointLight.specular * spec ;
   ambient  *= attenuation;
   diffuse  *= attenuation;
   specular *= attenuation;

   // 计算阴影
//   float shadow = ShadowCalculation(FragPosLightSpace);
//   float shadow = ShadowCalculation(FragPosLightSpace);
//   vec3 lighting = (ambient + (1.0 - 0.0) * (diffuse + specular)) * color;
   outColor = vec4((ambient + diffuse + specular) * color,1.0);

//   outColor = CalShadowAll();

}

vec4 CalShadowAll(){
   vec3 normal = normalize(v_normal);
   vec3 color = texture(material.diffuse, TexCoords).rgb;
   vec3 lightColor = vec3(1.0);
   // Ambient
   vec3 ambient = 0.15 * color;
   // Diffuse
//   vec3 lightDir = normalize(lightPos - FragPos);
   vec3 lightDir = normalize(vec3(-3.0,3.0,0.0));
   float diff = max(dot(lightDir, normal), 0.0);
   vec3 diffuse = diff * lightColor;
   // Specular
   vec3 viewDir = normalize(viewPos - FragPos);
   vec3 reflectDir = reflect(-lightDir, normal);
   float spec = 0.0;
   vec3 halfwayDir = normalize(lightDir + viewDir);
   spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
   vec3 specular = spec * lightColor;
   // 计算阴影
   float shadow = ShadowCalculation(FragPosLightSpace);
//   vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;

   vec3 lighting = (ambient + (1.0 - 0.0) * (diffuse + specular)) * color;

   vec4 result = vec4(lighting, 1.0f);
   return result;
}

/***
计算点光源
***/
vec3 CalcPointLight(PointLight pointLight, vec3 normal, vec3 FragPos, vec3 viewDir)
{
   // 这里用平行光
//   vec3 lightDir = normalize(pointLight.position - FragPos);
   vec3 lightDir = normalize(vec3(-3.0,3.0,-2.0));
   // 漫反射着色
   float diff = max(dot(normal, lightDir), 0.0);
   // 镜面光着色
   vec3 reflectDir = reflect(-lightDir, normal);
   float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
   // 衰减
   float distance    = length(pointLight.position - FragPos);
//   float attenuation = 1.0 / (pointLight.constant + pointLight.linear * distance +
//   pointLight.quadratic * (distance * distance));
   // 暂时不考虑衰减
   float attenuation = 1.0;

   // 合并结果
   vec3 ambient  = pointLight.lightColor * pointLight.ambient  * vec3(texture(material.diffuse, TexCoords));
   vec3 diffuse  = pointLight.lightColor * pointLight.diffuse  * diff * vec3(texture(material.diffuse, TexCoords));
   vec3 specular = pointLight.lightColor * pointLight.specular * spec * vec3(texture(material.specular, TexCoords));
   ambient  *= attenuation;
   diffuse  *= attenuation;
   specular *= attenuation;
   return (ambient + diffuse + specular);
}



float ShadowCalculation(vec4 fragPosLightSpace)
{
   // 执行透视除法
   vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
   // 变换到[0,1]的范围
   projCoords = projCoords * 0.5 + 0.5;
   // 取得最近点的深度(使用[0,1]范围下的fragPosLight当坐标)
   float closestDepth = texture(shadowMap, projCoords.xy).r;
   // 取得当前片段在光源视角下的深度
   float currentDepth = projCoords.z;
   // 检查当前片段是否在阴影中
   float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

   return shadow;
}