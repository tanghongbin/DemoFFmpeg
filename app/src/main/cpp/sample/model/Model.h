//
// Created by Admin on 2021/4/6.
//

#ifndef DEMOFFMPEG_MODEL_H
#define DEMOFFMPEG_MODEL_H

#include "../../glm/vec3.hpp"
#include "../../glm/vec2.hpp"
#include <stdlib.h>
#include <list>
#include <vector>
#include <string>
#include <assimp/mesh.h>
#include <assimp/scene.h>
#include "shader.h"
#include "Mesh.h"

/****
 * 三个assimp导入库
 */
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stb_image.h>


class Model{
public:
    Model(const char * path){
        loadModel(path);
    }
    ~Model(){
        for (Mesh &mesh : meshes) {
            mesh.Destroy();
        }
    }

    void draw(Shader* shader){
        for (int i = 0; i < meshes.size(); ++i) {
            meshes[i].draw(shader);
        }
//        LOGCATE("log total mesh size:%d",meshes.size());
    }

    float GetMaxViewDistance()
    {
        glm::vec3 vec3 = (abs(minXyz) + abs(maxXyz)) / 2.0f;
        float maxDis = fmax(vec3.x, fmax(vec3.y, vec3.z));
//        LOGCATE("Model::GetMaxViewDistance maxDis=%f", maxDis);
        return maxDis;
    }

    glm::vec3 GetAdjustModelPosVec()
    {
        glm::vec3 vec3 = (minXyz + maxXyz) / 2.0f;
//        LOGCATE("Model::GetAdjustModelPosVec vec3(%f, %f, %f)", vec3.x, vec3.y, vec3.z);
        return (minXyz + maxXyz) / 2.0f;
    }

private:
    std::vector<Mesh> meshes;
    std::string directory;
    std::vector<Texture> textures_loaded;
    glm::vec3 maxXyz, minXyz;
    /***
     * function
     */
     void loadModel(const char * path){
         int64_t startTime = GetSysCurrentTime();
         Assimp::Importer importer;
         const aiScene* scene = importer.ReadFile(path,aiProcess_Triangulate | aiProcess_FlipUVs);
         if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
             LOGCATE("import assimp error:%s",importer.GetErrorString());
             return;
         }
         std::string targetPath = std::string(path);
         directory = targetPath.substr(0,targetPath.find_last_of('/'));
         processNode(scene->mRootNode,scene);
         LOGCATE("load all complete, totalCost:%lld",(GetSysCurrentTime() - startTime));
     }

     void processNode(aiNode * node,const aiScene * scene) {
         for (int i = 0; i < node->mNumMeshes;  ++i) {
             aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
             meshes.push_back(processMesh(mesh,scene));
         }
         for (int i = 0; i < node->mNumChildren;++i) {
             processNode(node->mChildren[i],scene);
         }
     }

    void updateMaxMinXyz(glm::vec3 pos)
    {
        maxXyz.x = pos.x > maxXyz.x ? pos.x : maxXyz.x;
        maxXyz.y = pos.y > maxXyz.y ? pos.y : maxXyz.y;
        maxXyz.z = pos.z > maxXyz.z ? pos.z : maxXyz.z;

        minXyz.x = pos.x < minXyz.x ? pos.x : minXyz.x;
        minXyz.y = pos.y < minXyz.y ? pos.y : minXyz.y;
        minXyz.z = pos.z < minXyz.z ? pos.z : minXyz.z;
    }

     Mesh processMesh(aiMesh * mesh,const aiScene * scene) {
         std::vector<Vertex> vertices;
         std::vector<unsigned int> indices;
         std::vector<Texture> textures;

         for(unsigned int i = 0; i < mesh->mNumVertices; i++)
         {
             aiVector3D aIvertex = mesh->mVertices[i];
             aiVector3D aINormal = mesh->mNormals[i];
             Vertex vertex;
             vertex.Position = glm::vec3(aIvertex.x,aIvertex.y,aIvertex.z);
             updateMaxMinXyz(vertex.Position);
             vertex.Normal = glm::vec3(aINormal.x,aINormal.y,aINormal.z);
             if (mesh->mTextureCoords[0]){
                 vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x,mesh->mTextureCoords[0][i].y);
             } else {
                 vertex.TexCoords = glm::vec2 (0.0,0.0);
             }
             // 处理顶点位置、法线和纹理坐标
             vertices.push_back(vertex);
         }
         // 处理索引
         for (int i = 0; i < mesh->mNumFaces; ++i) {
             aiFace face = mesh->mFaces[i];
             for (int j = 0; j < face.mNumIndices; ++j) {
                 indices.push_back(face.mIndices[j]);
             }
         }
         // 处理材质
         if(mesh->mMaterialIndex >= 0){
             aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
             std::vector<Texture> diffuseMaps = loadMaterialTextures(material,
                                                                aiTextureType_DIFFUSE, "texture_diffuse");
             textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
             std::vector<Texture> specularMaps = loadMaterialTextures(material,
                                                                 aiTextureType_SPECULAR, "texture_specular");
             textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
         }
         for (int i = 0; i < textures.size(); ++i) {
             LOGCATE("log text path:%s id:%d",textures[i].path.C_Str(),textures[i].id);
         }

         return Mesh(vertices, indices, textures);
     }
     std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                         const char *typeName) {
         std::vector<Texture> textures;
         for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
         {
             aiString str;
             mat->GetTexture(type, i, &str);
             bool skip = false;
             for(unsigned int j = 0; j < textures_loaded.size(); j++)
             {
                 if(std::strcmp(textures_loaded[j].path.data, str.C_Str()) == 0)
                 {
                     textures.push_back(textures_loaded[j]);
                     skip = true;
                     break;
                 }
             }
             if(!skip)
             {   // 如果纹理还没有被加载，则加载它
                 Texture texture;
                 texture.id = TextureFromFile(str.C_Str(), directory);
                 texture.type = typeName;
                 texture.path = str.C_Str();
                 textures.push_back(texture);
                 textures_loaded.push_back(texture); // 添加到已加载的纹理中
             }
         }
         return textures;
     }

};





#endif //DEMOFFMPEG_MODEL_H
