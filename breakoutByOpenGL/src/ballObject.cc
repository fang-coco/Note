#include "ballObject.hh"
#include "gameObject.hh"

BallObject::BallObject()
    : GameObject(), Radius(12.5f), IsStuck(true), IsPassThrough(false),
      IsSticky(false) {}

BallObject::BallObject(glm::vec2 position, float radius, glm::vec2 velocity,
                       Texture2D sprite)
    : GameObject(position, glm::vec2(radius * 2.0f, radius * 2.0f), sprite,
                 glm::vec3(1.0f), velocity),
      Radius(radius), IsStuck(true), IsPassThrough(false), IsSticky(false) {}

glm::vec2 BallObject::Move(float dt, unsigned int windowWidth) {
  if (!this->IsStuck) {
    this->Position += this->Velocity * dt;
    if (this->Position.x <= 0.0f) {
      this->Velocity.x = -this->Velocity.x;
      this->Position.x = 0.0f;
    }
    if (this->Position.x + this->Size.x >= windowWidth) {
      this->Velocity.x = -this->Velocity.x;
      this->Position.x = windowWidth - this->Size.x;
    }
    if (this->Position.y <= 0.0f) {
      this->Velocity.y = -this->Velocity.y;
      this->Position.y = 0.0f;
    }
  }
  return this->Position;
}

void BallObject::Reset(glm::vec2 position, glm::vec2 velocity) {
  this->Color = glm::vec3(1.0f);
  this->Position = position;
  this->Velocity = velocity;
  this->IsStuck = true;
  this->IsSticky = false;
  this->IsPassThrough = false;
}
