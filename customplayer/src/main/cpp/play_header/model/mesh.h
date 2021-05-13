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
    std::string type;
    aiString path;
};


class Mesh{

private:
    uint32_t Vbo,Ebo;
    void setupMesh() {
        glGenVertexArrays(1,&Vao);
        glGenBuffers(1,&Vbo);
        glGenBuffers(1,&Ebo);
        glBindVertexArray(Vao);
        bindBufferInternal();
        glBindVertexArray(0);
    }

public:
    uint32_t Vao;
    std::vector<Vertex> vertices;
    std::vector<unsigned int > indices;
    std::vector<Texture> textures;

    Mesh(std::vector<Vertex> vertices,std::vector<unsigned int > indices,std::vector<Texture> textures) {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;
        setupMesh();

    }

    void bindBufferInternal() {
        glBindBuffer(GL_ARRAY_BUFFER, Vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), &indices[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void *)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void *)offsetof(Vertex,Normal));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void *)offsetof(Vertex,TexCoords));
    }

    void Draw(Shader* shader){
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
            shader -> setInt((name + number).c_str(),index);
            glBindTexture(GL_TEXTURE_2D,textures[index].id);
        }

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


};

#endif //DEMOFFMPEG_MESH_H
