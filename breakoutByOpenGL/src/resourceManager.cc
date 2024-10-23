#include "resourceManager.hh"
#include "glad/glad.h"
#include "stb_image/stb_image.h"
#include <fstream>
#include <iostream>
#include <sstream>

std::unordered_map<std::string, Shader> ResMager::Shaders;
std::unordered_map<std::string, Texture2D> ResMager::Textures;

Shader ResMager::LoadShader(const char *vShaderFile, const char *fShaderFile,
                            const char *gShaderFile, std::string name) {
  Shaders[name] = LoadShaderFromFile(vShaderFile, fShaderFile, gShaderFile);
  return Shaders[name];
}

Shader ResMager::GetShader(std::string name) { return Shaders[name]; }

Texture2D ResMager::LoadTexture(const char *file, std::string name) {
  Textures[name] = LoadTextureFromFile(file);

  return Textures[name];
}

Texture2D ResMager::GetTexture(std::string name) { return Textures[name]; }

Texture2D ResMager::LoadTextureFromFile(const char *file) {
  Texture2D texture;

  int width, height, channel;
  unsigned char *data = stbi_load(file, &width, &height, &channel, 0);
  unsigned int format;
  if (channel == 1)
    format = GL_RED;
  else if (channel == 3)
    format = GL_RGB;
  else if (channel == 4)
    format = GL_RGBA;
  texture.Internal_Format = format;
  texture.Image_Format = format;
  texture.Generate(width, height, data);
  stbi_image_free(data);
  return texture;
}

Shader ResMager::LoadShaderFromFile(const char *vShaderFile,
                                    const char *fShaderFile,
                                    const char *gShaderFile) {
  std::string vertexCode;
  std::string fragmentCode;
  std::string geometryCode;
  try {
    // open files
    std::ifstream vertexShaderFile(vShaderFile);
    std::ifstream fragmentShaderFile(fShaderFile);
    std::stringstream vShaderStream, fShaderStream;
    // read file's buffer contents into streams
    vShaderStream << vertexShaderFile.rdbuf();
    fShaderStream << fragmentShaderFile.rdbuf();
    // close file handlers
    vertexShaderFile.close();
    fragmentShaderFile.close();
    // convert stream into string
    vertexCode = vShaderStream.str();
    fragmentCode = fShaderStream.str();
    // if geometry shader path is present, also load a geometry shader
    if (gShaderFile != nullptr) {
      std::ifstream geometryShaderFile(gShaderFile);
      std::stringstream gShaderStream;
      gShaderStream << geometryShaderFile.rdbuf();
      geometryShaderFile.close();
      geometryCode = gShaderStream.str();
    }
  } catch (std::exception e) {
    std::cout << "ERROR::SHADER: Failed to read shader files" << std::endl;
  }
  const char *vShaderCode = vertexCode.c_str();
  const char *fShaderCode = fragmentCode.c_str();
  const char *gShaderCode = geometryCode.c_str();
  // 2. now create shader object from source code
  Shader shader;
  shader.Compile(vShaderCode, fShaderCode,
                 gShaderFile != nullptr ? gShaderCode : nullptr);
  return shader;
}
