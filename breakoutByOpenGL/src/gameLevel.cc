#include "gameLevel.hh"
#include "gameObject.hh"
#include "resourceManager.hh"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

void GameLevel::Load(const char *file, unsigned int levelWidth,
                     unsigned int levelHeight) {
  this->Bricks.clear();
  unsigned int tileCode;
  std::vector<std::vector<unsigned int>> tileData;
  std::string line;
  std::ifstream fs(file);

  if (fs) {
    while (std::getline(fs, line)) {
      std::istringstream ss(line);
      std::vector<unsigned int> row;
      while (ss >> tileCode) {
        row.push_back(tileCode);
      }
      tileData.push_back(row);
    }
    if (tileData.size() > 0)
      this->init(tileData, levelWidth, levelHeight);
  }
}

void GameLevel::Draw(SpriteRenderer &renderer) {
  for (auto &tile : this->Bricks) {
    if (!tile.Destroyed) {
      tile.Draw(renderer);
    }
  }
}

bool GameLevel::Iscompleted() {
  for (auto &tile : this->Bricks) {
    if (!tile.IsSolid && !tile.Destroyed) {
      return false;
    }
  }
  return true;
}

void GameLevel::init(std::vector<std::vector<unsigned int>> tileData,
                     unsigned int levelWidth, unsigned int levelHeight) {

  unsigned int height = tileData.size();
  unsigned int width = tileData[0].size();

  float unit_width = (float)levelWidth / width;
  float unit_height = (float)levelHeight / height;

  for (unsigned int y = 0; y < height; ++y) {
    for (unsigned int x = 0; x < width; ++x) {
      // check block type from level data (2D level array)
      if (tileData[y][x] == 1) // solid
      {
        glm::vec2 pos(unit_width * x, unit_height * y);
        glm::vec2 size(unit_width, unit_height);
        GameObject obj(pos, size, ResMager::GetTexture("block_solid"),
                       glm::vec3(0.8f, 0.8f, 0.7f));
        obj.IsSolid = true;
        this->Bricks.push_back(obj);
      } else if (tileData[y][x] > 1) {
        glm::vec3 color = glm::vec3(1.0f); // original: white
        if (tileData[y][x] == 2)
          color = glm::vec3(0.2f, 0.6f, 1.0f);
        else if (tileData[y][x] == 3)
          color = glm::vec3(0.0f, 0.7f, 0.0f);
        else if (tileData[y][x] == 4)
          color = glm::vec3(0.8f, 0.8f, 0.4f);
        else if (tileData[y][x] == 5)
          color = glm::vec3(1.0f, 0.5f, 0.0f);

        glm::vec2 pos(unit_width * x, unit_height * y);
        glm::vec2 size(unit_width, unit_height);
        this->Bricks.push_back(
            GameObject(pos, size, ResMager::GetTexture("block"), color));
      }
    }
  }
}
