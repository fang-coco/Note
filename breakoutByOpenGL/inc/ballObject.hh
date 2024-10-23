#pragma once
#include "gameObject.hh"
#include "texture.hh"
#include <sys/wait.h>

class BallObject : public GameObject {
public:
  float Radius;
  bool IsStuck;
  bool IsPassThrough;
  bool IsSticky;
  BallObject();
  BallObject(glm::vec2 position, float radius, glm::vec2 velocity,
             Texture2D sprite);

  glm::vec2 Move(float dt, unsigned int windowWidth);
  void Reset(glm::vec2 position, glm::vec2 velocity);
};
