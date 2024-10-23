#pragma once

#include "mesh.hh"
#include "shader.hh"
#include "stb_image.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

class Model {
public:
  std::vector<Texture> texture_loaded;
  std::vector<Mesh> meshes;
  std::string directory;
  bool gammaCorrection;
  Model(const std::string &path, bool gamma = true) : gammaCorrection(gamma) {
    loadModel(path);
  }

  void Draw(Shader &shader) {
    for (auto &mesh : meshes) {
      mesh.Draw(shader);
    }
  }

private:
  void loadModel(const std::string &path) {
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(
        path, aiProcess_Triangulate | aiProcess_GenSmoothNormals |
                  aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
        !scene->mRootNode) {
      std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
      return;
    }

    directory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
  }

  void processNode(aiNode *node, const aiScene *scene) {
    for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
      aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
      meshes.push_back(processMesh(mesh, scene));
    }
    for (unsigned int i = 0; i < node->mNumChildren; ++i) {
      processNode(node->mChildren[i], scene);
    }
  }

  Mesh processMesh(aiMesh *mesh, const aiScene *scene) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
      Vertex vertex;
      glm::vec3 vec;
      vec.x = mesh->mVertices[i].x;
      vec.y = mesh->mVertices[i].y;
      vec.z = mesh->mVertices[i].z;

      vertex.Position = vec;

      if (mesh->HasNormals()) {
        vec.x = mesh->mNormals[i].x;
        vec.y = mesh->mNormals[i].y;
        vec.z = mesh->mNormals[i].x;
        vertex.Normal = vec;
      }

      if (mesh->mTextureCoords[0]) {
        glm::vec2 v;
        v.x = mesh->mTextureCoords[0][i].x;
        v.y = mesh->mTextureCoords[0][i].y;
        vertex.Texcoords = v;

        vec.x = mesh->mTangents[i].x;
        vec.y = mesh->mTangents[i].y;
        vec.z = mesh->mTangents[i].z;
        vertex.Tangent = vec;

        vec.x = mesh->mBitangents[i].x;
        vec.y = mesh->mBitangents[i].y;
        vec.z = mesh->mBitangents[i].z;
        vertex.Bitangent = vec;
      } else {
        vertex.Texcoords = glm::vec2(0.0f, 0.0f);
      }

      vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
      aiFace face = mesh->mFaces[i];
      for (unsigned int j = 0; j < face.mNumIndices; ++j) {
        indices.push_back(face.mIndices[j]);
      }
    }

    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

    // 1. diffuse maps
    std::vector<Texture> diffuseMaps = loadMaterialTextures(
        material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    // 2. specular maps
    std::vector<Texture> specularMaps = loadMaterialTextures(
        material, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    // 3. normal maps
    std::vector<Texture> normalMaps =
        loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    // 4. height maps
    std::vector<Texture> heightMaps =
        loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

    return Mesh(vertices, indices, textures);
  }

  std::vector<Texture> loadMaterialTextures(aiMaterial *material,
                                            aiTextureType type,
                                            const std::string typeName) {
    std::vector<Texture> textures;
    for (unsigned int i = 0; i < material->GetTextureCount(type); ++i) {
      aiString str;
      material->GetTexture(type, i, &str);

      bool skip = false;
      for (auto &tex : texture_loaded) {
        if (std::strcmp(tex.path.data(), str.C_Str()) == 0) {
          textures.push_back(tex);
          skip = true;
          break;
        }
      }

      if (!skip) {
        Texture texture;
        texture.id = TextureFromFile(str.C_Str(), this->directory);
        texture.path = str.C_Str();
        texture.type = typeName;
        textures.push_back(texture);
        texture_loaded.push_back(texture);
      }
    }

    return textures;
  }

  unsigned int TextureFromFile(const char *path, const std::string &directory) {
    std::string filename = std::string(path);
    filename = directory + "/" + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponent;
    unsigned char *data =
        stbi_load(filename.c_str(), &width, &height, &nrComponent, 0);
    if (data) {
      GLenum format;
      if (nrComponent == 1)
        format = GL_RED;
      else if (nrComponent == 3)
        format = GL_RGB;
      else if (nrComponent == 4)
        format = GL_RGBA;

      glBindTexture(GL_TEXTURE_2D, textureID);
      glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                   GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                      GL_LINEAR_MIPMAP_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      stbi_image_free(data);
    } else {
      std::cout << "Texture to load at path: " << path << std::endl;
      stbi_image_free(data);
    }
    return textureID;
  }
};
