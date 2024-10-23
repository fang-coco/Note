#pragma once

#include <freetype2/ft2build.h>
#include FT_FREETYPE_H
#include "shader.hh"
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>
struct Character {
  unsigned int TextureID;
  glm::ivec2 Size;
  glm::ivec2 Bearing;
  FT_Pos Advance;
};

class TextRenderer {
public:
  std::unordered_map<char, Character> Characters;
  Shader TextShader;
  TextRenderer(unsigned int width, unsigned int height);

  void Load(const std::string &font, unsigned int fontSize);
  void RenderText(const std::string &text, float x, float y, float scale,
                  glm::vec3 color = glm::vec3(1.0f));

private:
  unsigned int VAO;
  unsigned int VBO;
};
