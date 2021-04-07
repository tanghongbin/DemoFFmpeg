//
// Created by Admin on 2021/4/7.
//

#ifndef DEMOFFMPEG_MESH_H
#define DEMOFFMPEG_MESH_H

#include "../../glm/vec3.hpp"
#include "../../glm/vec2.hpp"
#include <stdlib.h>
#include <list>
#include <vector>
#include <string>
#include <assimp/mesh.h>
#include <assimp/scene.h>
#include "shader.h"
#include "stb_image.h"

struct Vertex{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct Texture{
    unsigned int id;
    const char * type;
    aiString path;
};


class Mesh{
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int > indices;
    std::vector<Texture> textures;
//    GLuint imageTextureId;
    Mesh(std::vector<Vertex> vertices,std::vector<unsigned int > indices,std::vector<Texture> textures) {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;
        setupMesh();

        // test
//        const char *fileName = "/storage/emulated/0/ffmpegtest/3d_obj/apple/textures/Apricot_02_diffuse.png";
//        LoadImageInfo imageInfo;
//        stbi_uc *imageData = stbi_load(fileName, &imageInfo.width, &imageInfo.height, &imageInfo.channels, 0);
//        logLoadImageInfo(imageInfo);
//        glGenTextures(1,&imageTextureId);
//        glBindTexture(GL_TEXTURE_2D, imageTextureId);
//        glGenerateMipmap(GL_TEXTURE_2D);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageInfo.width,
//                     imageInfo.height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
//                     imageData);
//        glBindTexture(GL_TEXTURE_2D, 0);
//        LOGCATE("log texture own id:%d",imageTextureId);
////        GO_CHECK_GL_ERROR();
//        stbi_image_free(imageData);
    }

    void draw(Shader* shader){
        uint32_t diffuseNr = 1;
        uint32_t specularNr = 1;
        for (uint32_t index = 0;  index < textures.size(); index++) {
            glActiveTexture(GL_TEXTURE0 + index);
            std::string number;
            std::string name = textures[index].type;
            if (name == "texture_diffuse"){
                number = std::to_string(diffuseNr++);
            } else if (name == "texture_specular"){
                number = std::to_string(specularNr++);
            }
//            LOGCATE("log sampler :%s index:%d id:%d",(name + number).c_str(),index,textures[index].id);
            shader->setInt((name + number).c_str(),index);
            glBindBuffer(GL_TEXTURE_2D,textures[index].id);
        }
        if (textures.size() > 1) {
//            LOGCATE("every time texture size:%d name:%s name2:%s",textures.size(),textures[0].type
//            ,textures[1].type);
        } else {
//            LOGCATE("every time texture size:%d name:%s",textures.size(),textures[0].type);
        }
//        glActiveTexture(GL_TEXTURE0);
//        shader->setInt("texture_diffuse1",0);
//        glBindBuffer(GL_TEXTURE_2D,imageTextureId);

        glBindVertexArray(Vao);
        glDrawElements(GL_TRIANGLES,indices.size(),GL_UNSIGNED_INT,(void *)0);
        glBindVertexArray(0);

    }
    void Destroy(){
        for (int i = 0; i < textures.size(); ++i) {
            glDeleteTextures(1, &textures[i].id);
        }
        glDeleteBuffers(1, &Ebo);
        glDeleteBuffers(1, &Vbo);
        glDeleteVertexArrays(1, &Vao);
        Vao = Vbo = Ebo = GL_NONE;
    }

private:
    uint32_t Vao,Vbo,Ebo;
    void setupMesh() {
        glGenVertexArrays(1,&Vao);
        glGenBuffers(1,&Vbo);
        glGenBuffers(1,&Ebo);

        glBindVertexArray(Vao);
        glBindBuffer(GL_ARRAY_BUFFER,Vbo);
        glBufferData(GL_ARRAY_BUFFER,vertices.size() * sizeof(Vertex),&vertices[0],GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,Ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,indices.size() * sizeof(uint32_t),&indices[0],GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void *)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void *)offsetof(Vertex,Normal));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void *)offsetof(Vertex,TexCoords));
        glBindVertexArray(0);
    }
};

#endif //DEMOFFMPEG_MESH_H
