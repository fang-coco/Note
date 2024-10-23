#pragma once

#include "shader.hh"
#include "texture.hh"
class PostProcessor {
public:
  Shader PostProcessingShader;
  Texture2D Texture;
  unsigned int Width;
  unsigned int Height;

  bool Confuse;
  bool Chaos;
  bool Shake;

  PostProcessor(Shader shader, unsigned int width, unsigned int height);
  void BeginRender();
  void EndRender();
  void Render(float time);

private:
  unsigned int MSFBO;
  unsigned int FBO;
  unsigned int RBO;
  unsigned int VAO;
  void initRenderData();
};
