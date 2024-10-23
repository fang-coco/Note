#include "textRenderer.hh"
#include "glad/glad.h"
#include "resourceManager.hh"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <ostream>
#include <utility>

TextRenderer::TextRenderer(unsigned int width, unsigned int height) {

  // load and configure shader
  this->TextShader = ResMager::GetShader("text");
  this->TextShader.Use();
  this->TextShader.SetMatrix4(
      "projection", glm::ortho(0.0f, static_cast<float>(width),
                               static_cast<float>(height), 0.0f, -1.0f, 1.0f));
  this->TextShader.SetInteger("text", 0);
  // configure VAO/VBO for texture quads
  glGenVertexArrays(1, &this->VAO);
  glGenBuffers(1, &this->VBO);
  glBindVertexArray(this->VAO);
  glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void TextRenderer::Load(const std::string &font, unsigned int fontSize) {
  this->Characters.clear();
  FT_Library ft;
  if (FT_Init_FreeType(&ft)) {
    std::cout << "error::freetype2: init library failed!" << std::endl;
    return;
  }

  FT_Face face;
  if (FT_New_Face(ft, font.c_str(), 0, &face)) {
    std::cout << "error::freetype2: new face failed!" << std::endl;
    return;
  }

  FT_Set_Pixel_Sizes(face, 0, fontSize);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  for (GLubyte c = 0; c < 128; ++c) {
    if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
      std::cout << "error::freetype2: load char failed!" << std::endl;
      continue;
    }

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width,
                 face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE,
                 face->glyph->bitmap.buffer);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    Character character = {
        texture,
        glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
        glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
        face->glyph->advance.x};
    Characters.insert(std::make_pair(c, character));
  }
  glBindTexture(GL_TEXTURE_2D, 0);
  FT_Done_Face(face);
  FT_Done_FreeType(ft);
}
void TextRenderer::RenderText(const std::string &text, float x, float y,
                              float scale, glm::vec3 color) {

  this->TextShader.Use();
  this->TextShader.SetVector3f("textColor", color);
  glActiveTexture(GL_TEXTURE0);
  glBindVertexArray(this->VAO);

  for (auto &c : text) {
    auto ch = Characters[c];

    // std::cout << "debug::freetype2: Character: " << c << " >>> " <<
    // ch.TextureID
    //           << std::endl;
    float xpos = x + ch.Bearing.x * scale;
    float ypos = y + (this->Characters['H'].Bearing.y - ch.Bearing.y) * scale;

    float w = ch.Size.x * scale;
    float h = ch.Size.y * scale;
    // update VBO for each character
    float vertices[6][4] = {
        {xpos, ypos + h, 0.0f, 1.0f}, {xpos + w, ypos, 1.0f, 0.0f},
        {xpos, ypos, 0.0f, 0.0f},

        {xpos, ypos + h, 0.0f, 1.0f}, {xpos + w, ypos + h, 1.0f, 1.0f},
        {xpos + w, ypos, 1.0f, 0.0f}};
    // render glyph texture over quad
    glBindTexture(GL_TEXTURE_2D, ch.TextureID);
    // update content of VBO memory
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferSubData(
        GL_ARRAY_BUFFER, 0, sizeof(vertices),
        vertices); // be sure to use glBufferSubData and not glBufferData
    // render quad
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // now advance cursors for next glyph
    x += (ch.Advance >> 6) *
         scale; // bitshift by 6 to get value in pixels (1/64th times 2^6 = 64)
  }
  glBindTexture(GL_TEXTURE_2D, 0);
  glBindVertexArray(0);
}
