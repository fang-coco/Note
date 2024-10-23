#pragma once

#include <freetype2/ft2build.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include FT_FREETYPE_H

struct Character {
  GLuint TextureID;
  glm::ivec2 Size;
  glm::ivec2 Bearing;
  GLint64 Advance;
};

class Fonts {
public:
  Fonts(const char *path_ttf, glm::ivec2 pixel) {
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
      std::cout << "ERROR::FREETYPE: Could not init FreeType Library"
                << std::endl;

    FT_Face face;
    if (FT_New_Face(ft, path_ttf, 0, &face))
      std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

    FT_Set_Pixel_Sizes(face, pixel.x, pixel.y);
    this->Face = face;
    this->Ft = ft;
  }

  ~Fonts() {
    FT_Done_Face(this->Face);
    FT_Done_FreeType(this->Ft);
  }

  void loadCharsByStr(const std::string &str) {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    for (auto c : str) {
      if (FT_Load_Char(this->Face, c, FT_LOAD_RENDER)) {
        std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
        continue;
      }
      GLuint texture;
      glGenTextures(1, &texture);
      glBindTexture(GL_TEXTURE_2D, texture);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, this->Face->glyph->bitmap.width,
                   this->Face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE,
                   this->Face->glyph->bitmap.buffer);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      Character character{texture,
                          glm::ivec2(this->Face->glyph->bitmap.width,
                                     this->Face->glyph->bitmap.rows),
                          glm::ivec2(this->Face->glyph->bitmap_left,
                                     this->Face->glyph->bitmap_top),
                          this->Face->glyph->advance.x};
      this->Characters.insert(std::make_pair(c, character));
    }
  }

  const Character &getChar(const GLchar ch) { return Characters[ch]; }

private:
  FT_Face Face;
  FT_Library Ft;
  std::unordered_map<GLchar, Character> Characters;
};
