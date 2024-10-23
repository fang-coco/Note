#pragma once
#include "gameObject.hh"
#include "shader.hh"
#include "texture.hh"
#include <glm/glm.hpp>
#include <vector>

struct Particle {
  glm::vec2 Position;
  glm::vec2 Velocity;
  glm::vec4 Color;
  float Live;
  Particle() : Position(0.0f), Velocity(0.0f), Color(1.0f), Live(1.0f) {}
};

class ParticleGenerator {

public:
  ParticleGenerator(Shader shader, Texture2D texture, unsigned int amount);
  void Update(float dt, GameObject &object, unsigned int newParticles,
              glm::vec2 offest = glm::vec2(0.0f));
  void Draw();

private:
  std::vector<Particle> particles;
  unsigned int amount;
  Shader shader;
  Texture2D texture;
  unsigned int VAO;
  void init();
  unsigned int firstUnusedParticle();
  void respawnParticle(Particle &particle, GameObject &object,
                       glm::vec2 offest = glm::vec2(0.0f));
};
