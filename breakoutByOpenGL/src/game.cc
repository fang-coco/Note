#include "game.hh"
#include "GLFW/glfw3.h"
#include "audioPlayer.hh"
#include "ballObject.hh"
#include "gameLevel.hh"
#include "gameObject.hh"
#include "glm/gtc/matrix_transform.hpp"
#include "particleGenerator.hh"
#include "postProcessor.hh"
#include "powerUp.hh"
#include "resourceManager.hh"
#include "spriteRenderer.hh"
#include "textRenderer.hh"
#include <OpenGL/gl.h>
#include <cmath>
#include <cstdlib>
#include <string>
#include <vector>

SpriteRenderer *Renderer;

GameObject *Player;

BallObject *Ball;

ParticleGenerator *Particles;

PostProcessor *Effects;

AudioPlayer *ALPlayer;

TextRenderer *Texter;

Game::Game(unsigned int width, unsigned int height)
    : State(GAME_ACTIVE), Keys(), Live(3) {
  this->Width = width;
  this->Height = height;
}
Game::~Game() {
  delete Renderer;
  delete Player;
  delete Ball;
  delete Effects;
}

void Game::Init() {

  ResMager::LoadShader("../shaders/spriteShader.vs",
                       "../shaders/spriteShader.fs", nullptr, "sprite");

  ResMager::LoadShader("../shaders/particle.vs", "../shaders/particle.fs",
                       nullptr, "particle");

  ResMager::LoadShader("../shaders/postProcessor.vs",
                       "../shaders/postProcessor.fs", nullptr, "postProcessor");

  ResMager::LoadShader("../shaders/textRenderer.vs",
                       "../shaders/textRenderer.fs", nullptr, "text");
  glm::mat4 projection =
      glm::ortho(0.0f, static_cast<float>(this->Width),
                 static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);

  ResMager::GetShader("sprite").Use().SetInteger("Image", 0);
  ResMager::GetShader("sprite").SetMatrix4("projection", projection);
  Renderer = new SpriteRenderer(ResMager::GetShader("sprite"));

  ResMager::LoadTexture("../res/textures/awesomeface.png", "face");
  ResMager::LoadTexture("../res/textures/block_solid.png", "block_solid");
  ResMager::LoadTexture("../res/textures/block.png", "block");
  ResMager::LoadTexture("../res/textures/background.jpg", "background");
  ResMager::LoadTexture("../res/textures/paddle.png", "paddle");
  ResMager::LoadTexture("../res/textures/particle.png", "particle");
  ResMager::LoadTexture("../res/textures/powerup_sticky.png", "sticky");
  ResMager::LoadTexture("../res/textures/powerup_passthrough.png",
                        "pass-through");
  ResMager::LoadTexture("../res/textures/powerup_speed.png", "speed");
  ResMager::LoadTexture("../res/textures/powerup_chaos.png", "chaos");
  ResMager::LoadTexture("../res/textures/powerup_confuse.png", "confuse");
  ResMager::LoadTexture("../res/textures/powerup_increase.png",
                        "pad-size-increase");

  ResMager::GetShader("particle").Use().SetMatrix4("projection", projection);
  ResMager::GetShader("particle").SetInteger("sprite", 0);

  Particles =
      new ParticleGenerator(ResMager::GetShader("particle"),
                            ResMager::GetTexture("particle"), PARTICLE_AMOUNT);

  glm::vec2 playPos(this->Width * 0.5f - PLAYER_SIZE.x * 0.5f,
                    this->Height - PLAYER_SIZE.y);
  Player = new GameObject(playPos, PLAYER_SIZE, ResMager::GetTexture("paddle"));

  glm::vec2 ballPos(
      Player->Position +
      glm::vec2(PLAYER_SIZE.x * 0.5 - BALL_RADIUS, -BALL_RADIUS * 2.0f));
  Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY,
                        ResMager::GetTexture("face"));

  Effects = new PostProcessor(ResMager::GetShader("postProcessor"),
                              this->Width * 2.0f, this->Height * 2.0f);

  GameLevel one;
  one.Load("../res/levels/one.lvl", this->Width, this->Height * 0.5);
  this->Levels.push_back(one);
  GameLevel two;
  two.Load("../res/levels/two.lvl", this->Width, this->Height * 0.5);
  this->Levels.push_back(two);
  GameLevel three;
  three.Load("../res/levels/three.lvl", this->Width, this->Height * 0.5);
  this->Levels.push_back(three);
  GameLevel four;
  four.Load("../res/levels/four.lvl", this->Width, this->Height * 0.5);
  this->Levels.push_back(four);
  this->Level = 2;

  ALPlayer = new AudioPlayer();
  ALPlayer->CreateSource("../res/audio/breakout.mp3", "breakout");
  ALPlayer->CreateSource("../res/audio/bleep.mp3", "bleep");
  ALPlayer->CreateSource("../res/audio/solid.wav", "solid");
  ALPlayer->CreateSource("../res/audio/powerup.wav", "powerup");
  ALPlayer->CreateSource("../res/audio/gameover.wav", "gameover");
  ALPlayer->CreateSource("../res/audio/winning.wav", "win");
  ALPlayer->CreateSource("../res/audio/menu.wav", "menu");
  ALPlayer->CreateSource("../res/audio/chose.wav", "chose");
  ALPlayer->Play("breakout", true);

  Texter = new TextRenderer(this->Width, this->Height);
  Texter->Load("../res/fonts/Karla-Regular.ttf", 20);
}

Direction VectorDirection(glm::vec2 target) {
  glm::vec2 compass[] = {glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 0.0f),
                         glm::vec2(0.0f, -1.0f), glm::vec2(-1.0f, 0.0f)};
  float max = -1.0f;
  int best_match = -1;
  for (int i = 0; i < 4; ++i) {
    float dot_product = glm::dot(glm::normalize(target), compass[i]);
    if (dot_product > max) {
      max = dot_product;
      best_match = i;
    }
  }
  // std::cout << "debug::best_match: " << best_match << std::endl;
  return (Direction)best_match;
}

typedef std::tuple<bool, Direction, glm::vec2> Collision;

Collision CheckCollision(BallObject &one,
                         GameObject &two) // AABB - Circle collision
{
  // get center point circle first
  glm::vec2 center(one.Position + one.Radius);
  // calculate AABB info (center, half-extents)
  glm::vec2 aabb_half_extents(two.Size.x / 2.0f, two.Size.y / 2.0f);
  glm::vec2 aabb_center(two.Position.x + aabb_half_extents.x,
                        two.Position.y + aabb_half_extents.y);
  // get difference vector between both centers
  glm::vec2 difference = center - aabb_center;
  glm::vec2 clamped =
      glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
  // add clamped value to AABB_center and we get the value of box closest to
  // circle
  glm::vec2 closest = aabb_center + clamped;
  // retrieve vector between center circle and closest point AABB and check if
  // length <= radius
  difference = closest - center;
  if (glm::length(difference) <= one.Radius) {
    return std::make_tuple(true, VectorDirection(difference), difference);
  } else {
    return std::make_tuple(false, Direction::UP, glm::vec2(0.0f, 0.0f));
  }
}

bool CheckCollision(GameObject &one, GameObject &two) {
  // collision x-axis?
  bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
                    two.Position.x + two.Size.x >= one.Position.x;
  // collision y-axis?
  bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
                    two.Position.y + two.Size.y >= one.Position.y;
  // collision only if on both axes
  return collisionX && collisionY;
}

float ShakeTime = 0.0f;

void Game::DoCollisions() {
  for (auto &obj : this->Levels[this->Level].Bricks) {
    if (!obj.Destroyed) {
      auto collision = CheckCollision(*Ball, obj);
      if (std::get<0>(collision)) {
        if (!obj.IsSolid) {
          ALPlayer->Play("bleep");
          obj.Destroyed = true;
          this->SpawnPowerUp(obj);
        } else {
          ALPlayer->Play("solid");
          ShakeTime = 0.05f;
          Effects->Shake = true;
        }
        auto dir = std::get<1>(collision);
        auto dif = std::get<2>(collision);
        if (!(Ball->IsPassThrough && !obj.IsSolid)) {
          if (dir == Direction::LEFT || dir == Direction::RIGHT) {
            Ball->Velocity.x = -Ball->Velocity.x;
            float penetration = Ball->Radius - std::abs(dif.x);
            if (dir == Direction::LEFT) {
              Ball->Position.x += penetration;
            } else {
              Ball->Position.x -= penetration;
            }
          } else {
            Ball->Velocity.y = -Ball->Velocity.y;
            float penetration = Ball->Radius - std::abs(dif.y);
            if (dir == Direction::UP) {
              Ball->Position.y -= penetration;
            } else {
              Ball->Position.y += penetration;
            }
          }
        }
      }
    }
  }

  auto result = CheckCollision(*Ball, *Player);
  if (!Ball->IsStuck && std::get<0>(result)) {
    float centerBoard = Player->Position.x + Player->Size.x * 0.5;
    float distance = Ball->Position.x + Ball->Radius - centerBoard;
    float penetration = distance / (Player->Size.x * 0.5);

    // std::cout << "debug::penetration: " << penetration << std::endl;
    float strength = 2.0f;
    auto oldVelocity = Ball->Velocity;
    Ball->Velocity.x = INITIAL_BALL_VELOCITY.x * penetration * strength;
    Ball->Velocity.y = -1.0f * std::abs(Ball->Velocity.y);
    Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity);
    Ball->IsStuck = Ball->IsSticky;
  }

  for (auto &powerUp : PowerUps) {
    if (!powerUp.Destroyed) {
      if (powerUp.Position.y >= this->Height) {
        powerUp.Destroyed = true;
      }
      if (CheckCollision(powerUp, *Player)) {
        ALPlayer->Play("powerup");
        this->ActivatePowerUp(powerUp);
        powerUp.Destroyed = true;
        powerUp.Activated = true;
      }
    }
  }
}

void Game::ProcessInput(float dt) {
  if (this->State == GAME_ACTIVE) {
    float velcoity = dt * PLAYER_VELOCITY;
    if (this->Keys[GLFW_KEY_A]) {
      if (Player->Position.x >= 0.0f) {
        Player->Position.x -= velcoity;
        if (Ball->IsStuck) {
          Ball->Position.x -= velcoity;
        }
      }
    }
    if (this->Keys[GLFW_KEY_D]) {
      if (Player->Position.x <= this->Width - Player->Size.x) {
        Player->Position.x += velcoity;
        if (Ball->IsStuck) {
          Ball->Position.x += velcoity;
        }
      }
    }
    if (this->Keys[GLFW_KEY_SPACE]) {
      Ball->IsStuck = false;
    }
  }
  if (this->State == GAME_ENUM) {
    if (this->Keys[GLFW_KEY_ENTER] && !this->KeysProcessed[GLFW_KEY_ENTER]) {
      this->State = GAME_ACTIVE;
      this->KeysProcessed[GLFW_KEY_ENTER] = GL_TRUE;
      ALPlayer->Display("menu");
      ALPlayer->Play("breakout", true);
    }
    if (this->Keys[GLFW_KEY_W] && !this->KeysProcessed[GLFW_KEY_W]) {
      this->Level = (this->Level + 1) % 4;
      this->KeysProcessed[GLFW_KEY_W] = GL_TRUE;
      ALPlayer->Play("chose");
    }
    if (this->Keys[GLFW_KEY_S] && !this->KeysProcessed[GLFW_KEY_S]) {
      if (this->Level > 0)
        --this->Level;
      else
        this->Level = 3;
      this->KeysProcessed[GLFW_KEY_S] = GL_TRUE;
      ALPlayer->Play("chose");
    }
  }
  if (this->State == GAME_WIN) {
    if (this->Keys[GLFW_KEY_ENTER]) {
      this->KeysProcessed[GLFW_KEY_ENTER] = GL_TRUE;
      Effects->Chaos = GL_FALSE;
      this->State = GAME_ENUM;
      ALPlayer->Display("win");
      ALPlayer->Play("menu");
    }
  }
}

void Game::Update(float dt) {
  Ball->Move(dt, this->Width);
  this->DoCollisions();
  Particles->Update(dt, *Ball, 3, glm::vec2(Ball->Radius * 0.5f));
  this->UpdatePowerUp(dt);

  if (ShakeTime > 0.0f) {
    ShakeTime -= dt;
    if (ShakeTime <= 0.0f) {
      Effects->Shake = false;
    }
  }

  if (Ball->Position.y >= this->Height) {
    --this->Live;
    if (this->Live == 0) {
      this->ResetLevel();
      this->State = GameState::GAME_ENUM;
      ALPlayer->Display("breakout");
      ALPlayer->Play("gameover");
      ALPlayer->Play("menu", true);
    }
    this->ResetPlayer();
  }

  if (this->State == GAME_ACTIVE && this->Levels[this->Level].Iscompleted()) {
    Effects->Chaos = GL_TRUE;
    this->State = GAME_WIN;
    ALPlayer->Display("breakout");
    ALPlayer->Play("win", true);
  }
}

void Game::Render() {
  if (this->State == GAME_ACTIVE || this->State == GAME_ENUM ||
      this->State == GAME_WIN) {
    Effects->BeginRender();
    Renderer->DrawSprite(ResMager::GetTexture("background"),
                         glm::vec2(0.0f, 0.0f),
                         glm::vec2(this->Width, this->Height), 0.0f);
    this->Levels[this->Level].Draw(*Renderer);
    Particles->Draw();
    Player->Draw(*Renderer);
    Ball->Draw(*Renderer);
    for (auto &p : this->PowerUps) {
      if (!p.Destroyed)
        p.Draw(*Renderer);
    }
    Texter->RenderText("Live: " + std::to_string(this->Live), 5.0f, 5.0f, 1.0f);
    Texter->RenderText("Level: " + std::to_string(this->Level), 5.0f, 20.0f,
                       1.0f);
    Effects->EndRender();
    Effects->Render(static_cast<float>(glfwGetTime()));
  }
  if (this->State == GameState::GAME_ENUM) {
    Texter->RenderText("Press ENTER to start", 250.0f, this->Height * 0.5f,
                       1.0f);
    Texter->RenderText("Press W or S to select level", 245.0f,
                       this->Height * 0.5f + 20.0f, 0.75f);
  }
  if (this->State == GAME_WIN) {
    Texter->RenderText("You WON!!!", 320.0, this->Height * 0.5 - 20.0, 1.0,
                       glm::vec3(0.0, 1.0, 0.0));
    Texter->RenderText("Press ENTER to retry or ESC to quit", 130.0,
                       this->Height * 0.5f, 1.0, glm::vec3(1.0, 1.0, 0.0));
  }
}

void Game::ResetLevel() {
  if (this->Level == 0)
    this->Levels[this->Level].Load("../res/levels/one.lvl", this->Width,
                                   this->Height * 0.5);
  else if (this->Level == 1)
    this->Levels[this->Level].Load("../res/levels/two.lvl", this->Width,
                                   this->Height * 0.5);
  else if (this->Level == 2)
    this->Levels[this->Level].Load("../res/levels/three.lvl", this->Width,
                                   this->Height * 0.5);
  else if (this->Level == 3)
    this->Levels[this->Level].Load("../res/levels/four.lvl", this->Width,
                                   this->Height * 0.5);
  this->Live = 3;
}

void Game::ResetPlayer() {
  Player->Size = PLAYER_SIZE;
  Player->Position = glm::vec2(this->Width * 0.5f - Player->Size.x * 0.5f,
                               this->Height - Player->Size.y);
  Ball->Reset(Player->Position + glm::vec2(Player->Size.x * 0.5f - BALL_RADIUS,
                                           -BALL_RADIUS * 2.0f),
              INITIAL_BALL_VELOCITY);
  Effects->Confuse = false;
  Effects->Chaos = false;

  this->PowerUps.clear();
}

bool ShouldSpawn(unsigned int chance) { return (rand() % chance) == 0; }

void Game::SpawnPowerUp(GameObject &block) {
  // 假如一次产生多个道具，会叠在一起。
  // 根据原始位置，添加一个偏移
  auto pos = block.Position;
  if (ShouldSpawn(75)) {
    auto offset = block.Size * ((float)(rand() % 100) / 100);
    this->PowerUps.push_back(PowerUp("speed", glm::vec3(0.5f, 0.5f, 1.0f), 0.0f,
                                     pos + offset,
                                     ResMager::GetTexture("speed")));
  }
  if (ShouldSpawn(75)) {
    auto offset = block.Size * ((float)(rand() % 100) / 100);
    this->PowerUps.push_back(PowerUp("sticky", glm::vec3(1.0f, 0.5f, 1.0f),
                                     10.0f, pos + offset,
                                     ResMager::GetTexture("sticky")));
  }
  if (ShouldSpawn(75)) {
    auto offset = block.Size * ((float)(rand() % 100) / 100);
    this->PowerUps.push_back(
        PowerUp("pass-through", glm::vec3(0.5f, 1.0f, 0.5f), 10.0f,
                pos + offset, ResMager::GetTexture("pass-through")));
  }
  if (ShouldSpawn(75)) {
    auto offset = block.Size * ((float)(rand() % 100) / 100);
    this->PowerUps.push_back(
        PowerUp("pad-size-increase", glm::vec3(1.0f, 0.6f, 0.4f), 5.0f,
                pos + offset, ResMager::GetTexture("pad-size-increase")));
  }
  if (ShouldSpawn(15)) {
    auto offset = block.Size * ((float)(rand() % 100) / 100);
    this->PowerUps.push_back(PowerUp("confuse", glm::vec3(1.0f, 0.3f, 0.3f),
                                     10.0f, pos + offset,
                                     ResMager::GetTexture("confuse")));
  }
  if (ShouldSpawn(15)) {
    auto offset = block.Size * ((float)(rand() % 100) / 100);
    this->PowerUps.push_back(PowerUp("chaos", glm::vec3(0.9f, 0.25f, 0.25f),
                                     10.0f, pos + offset,
                                     ResMager::GetTexture("chaos")));
  }
}

void Game::ActivatePowerUp(PowerUp &powerUp) {
  if (powerUp.Type == "speed") {
    Ball->Velocity *= 1.2f;
  } else if (powerUp.Type == "sticky") {
    Ball->IsSticky = true;
    Player->Color = glm::vec3(1.0f, 0.5f, 1.0f);
  } else if (powerUp.Type == "pass-through") {
    Ball->IsPassThrough = true;
    Ball->Color = glm::vec3(1.0f, 0.5f, 0.5f);
  } else if (powerUp.Type == "pad-size-increase") {
    Player->Size.x *= 2.0f;
  } else if (powerUp.Type == "confuse") {
    if (!Effects->Chaos) {
      Effects->Confuse = true;
    }
  } else if (powerUp.Type == "chaos") {
    if (!Effects->Confuse) {
      Effects->Chaos = true;
    }
  }
}

bool IsOtherPowerUpActive(std::vector<PowerUp> &vecs, std::string type) {
  for (auto &p : vecs) {
    if (p.Activated && p.Type == type)
      return true;
  }
  return false;
}

void Game::UpdatePowerUp(float dt) {
  for (auto &p : this->PowerUps) {
    p.Position += p.Velocity * dt;
    if (p.Activated) {
      p.Duration -= dt;
      if (p.Duration <= 0.0f) {
        p.Activated = false;
        if (p.Type == "sticky") {
          if (!IsOtherPowerUpActive(this->PowerUps, p.Type)) {
            Ball->IsSticky = false;
            Player->Color = glm::vec3(1.0f);
          }
        } else if (p.Type == "pass-through") {
          if (!IsOtherPowerUpActive(this->PowerUps, p.Type)) {
            Ball->IsPassThrough = false;
            Ball->Color = glm::vec3(1.0f);
          }
        } else if (p.Type == "pad-size-increase") {
          if (!IsOtherPowerUpActive(this->PowerUps, p.Type)) {
            Player->Size.x *= 0.5f;
          }
        } else if (p.Type == "confuse") {
          if (!IsOtherPowerUpActive(this->PowerUps, p.Type)) {
            Effects->Confuse = false;
          }
        } else if (p.Type == "chaos") {
          if (!IsOtherPowerUpActive(this->PowerUps, p.Type)) {
            Effects->Chaos = false;
          }
        }
      }
    }
  }

  this->PowerUps.erase(
      std::remove_if(this->PowerUps.begin(), this->PowerUps.end(),
                     [](const PowerUp &powerUp) {
                       return powerUp.Destroyed && !powerUp.Activated;
                     }),
      this->PowerUps.end());
}
