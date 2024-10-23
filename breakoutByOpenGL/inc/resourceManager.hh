#pragma once

#include "shader.hh"
#include "texture.hh"
#include <string>
#include <unordered_map>
class ResMager {

public:
  static std::unordered_map<std::string, Shader> Shaders;
  static std::unordered_map<std::string, Texture2D> Textures;

  static Shader LoadShader(const char *vShaderFile, const char *fShaderFile,
                           const char *gShaderFile, std::string name);
  static Shader GetShader(std::string name);

  static Texture2D LoadTexture(const char *file, std::string name);
  static Texture2D GetTexture(std::string name);

private:
  static Texture2D LoadTextureFromFile(const char *file);
  static Shader LoadShaderFromFile(const char *vShaderFile,
                                   const char *fShaderFile,
                                   const char *gShaderFile = nullptr);
  ResMager() {};
};
