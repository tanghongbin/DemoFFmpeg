#version 300 es                              
precision mediump float;   
in vec2 outTexCoords;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

out vec4 fragColor;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;

struct Material{
   float shininess;
   float ambient;
   float diffuse;
   float specular;
};

uniform Material material;

void main()                                  
{
   vec3 lightDir = normalize(lightPos - FragPos);
   // 计算环境光照
   vec3 ambient = material.ambient * lightColor;
   vec3 norm = normalize(Normal);
   // 计算漫反射角度
   float diff= max(dot(norm,lightDir),0.0);
   vec3 diffuse = material.diffuse * diff * lightColor;
   // 计算镜面强度
   vec3 viewDir = normalize(viewPos - FragPos);
   vec3 reflectDir = reflect(-lightDir,norm);
   float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
   vec3 specular =  material.specular * spec  * vec3(texture(texture_specular1,outTexCoords));
   vec3 result =  (ambient + diffuse + specular) * vec3(texture(texture_diffuse1,outTexCoords));
//   fragColor = vec4(vec3(texture(texture_diffuse1,outTexCoords)),1.0);
   fragColor = vec4(result,1.0);
}