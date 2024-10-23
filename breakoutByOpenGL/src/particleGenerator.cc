#include "particleGenerator.hh"
#include "glad/glad.h"
#include "shader.hh"
#include "texture.hh"
#include <OpenGL/gl.h>
#include <_stdlib.h>

ParticleGenerator::ParticleGenerator(Shader shader, Texture2D texture,
                                     unsigned int amount)
    : shader(shader), texture(texture), amount(amount) {
  this->init();
}
void ParticleGenerator::Update(float dt, GameObject &object,
                               unsigned int newParticles, glm::vec2 offest) {
  for (unsigned int i = 0; i < newParticles; ++i) {
    auto unusedParticle = this->firstUnusedParticle();
    this->respawnParticle(this->particles[unusedParticle], object, offest);
  }
  for (auto &p : this->particles) {
    p.Live -= dt;
    if (p.Live > 0.0f) {
      p.Color.a -= dt * 2.5f;
      p.Position -= p.Velocity * dt;
    }
  }
}

void ParticleGenerator::Draw() {
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  this->shader.Use();
  for (auto &particle : this->particles) {
    if (particle.Live > 0.0f) {
      this->shader.SetVector2f("offset", particle.Position);
      this->shader.SetVector4f("color", particle.Color);
      this->texture.Bind();
      glBindVertexArray(this->VAO);
      glDrawArrays(GL_TRIANGLES, 0, 6);
      glBindVertexArray(0);
    }
  }
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ParticleGenerator::init() {
  unsigned int VBO;
  float particle_quad[] = {0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
                           0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
                           1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f};

  glGenVertexArrays(1, &this->VAO);
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad,
               GL_STATIC_DRAW);

  glBindVertexArray(this->VAO);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  for (unsigned int i = 0; i < this->amount; ++i) {
    this->particles.push_back(Particle());
  }
}
void ParticleGenerator::respawnParticle(Particle &particle, GameObject &object,
                                        glm::vec2 offest) {
  float random = ((rand() % 100) - 50) / 10.0f;
  float rColor = 0.5f + ((rand() % 100) / 100.0f);
  particle.Position = object.Position + random + offest;
  particle.Color = glm::vec4(rColor, rColor, rColor, 1.0f);
  particle.Live = 1.0f;
  particle.Velocity = object.Velocity * 0.1f;
}

unsigned int lastUsedParticle = 0;
unsigned int ParticleGenerator::firstUnusedParticle() {
  for (unsigned int i = lastUsedParticle; i < this->amount; ++i) {
    if (this->particles[i].Live <= 0.0f) {
      return i;
    }
  }
  for (unsigned int i = 0; i < lastUsedParticle; ++i) {
    if (this->particles[i].Live <= 0.0f) {
      return i;
    }
  }
  lastUsedParticle = 0;
  return 0;
}
