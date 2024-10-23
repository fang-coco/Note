#pragma once

#include "gameLevel.hh"
#include "gameObject.hh"
#include "powerUp.hh"
#include <vector>
const unsigned int KEY_MAX_COUNT = 1024;
enum GameState { GAME_ACTIVE, GAME_ENUM, GAME_WIN };
enum Direction { UP, RIGHT, DOWN, LEFT };

// Initial size of the player paddle
const glm::vec2 PLAYER_SIZE(100.0f, 20.0f);
// Initial velocity of the player paddle
const float PLAYER_VELOCITY(500.0f);

// Initial velocity of the Ball
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
// Radius of the ball object
const float BALL_RADIUS = 12.5f;

const unsigned int PARTICLE_AMOUNT = 500;

class Game {

public:
  GameState State;
  bool Keys[KEY_MAX_COUNT];
  bool KeysProcessed[KEY_MAX_COUNT];
  std::vector<GameLevel> Levels;
  std::vector<PowerUp> PowerUps;
  unsigned int Level;
  unsigned int Width;
  unsigned int Height;
  unsigned int Live;
  Game(unsigned int width, unsigned int height);
  ~Game();

  void Init();
  void ProcessInput(float dt);
  void Update(float dt);
  void Render();

private:
  void DoCollisions();
  void ResetLevel();
  void ResetPlayer();
  void UpdatePowerUp(float dt);
  void SpawnPowerUp(GameObject &block);
  void ActivatePowerUp(PowerUp &powerUp);
};
