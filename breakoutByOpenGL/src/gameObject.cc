#include "gameObject.hh"
#include "spriteRenderer.hh"

GameObject::GameObject()
    : Position(0.0f, 0.0f), Size(1.0f, 1.0f), Velocity(0.0f, 0.0f),
      Color(1.0f, 1.0f, 1.0f), Rotation(0.0f), IsSolid(false), Destroyed(false),
      Sprite() {}

GameObject::GameObject(glm::vec2 position, glm::vec2 size, Texture2D sprite,
                       glm::vec3 color, glm::vec2 velocity)
    : Position(position), Size(size), Velocity(velocity), Color(color),
      Rotation(0.0f), IsSolid(false), Destroyed(false), Sprite(sprite) {}

void GameObject::Draw(SpriteRenderer &renderer) {
  renderer.DrawSprite(this->Sprite, this->Position, this->Size, this->Rotation,
                      this->Color);
}
