#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <random>
#include <stdexcept>

#include "backend.h"
namespace s21::snake {

Engine::~Engine() {
  recomputeLevelAndSpeed();
  UpdateBest();
}

Engine::Engine(Config cfg) : cfg_(cfg) {
  if (cfg_.width <= 3 || cfg_.height <= 3)
    throw std::invalid_argument("Board too small");

  best_path_ = cfg_.best_path.empty() ? defaultBestPath() : cfg_.best_path;
  best_ = loadBestFromFile(best_path_);

  grid_.assign(W() * H(), Cell::kEmpty);
  placeInitialSnake();

  rng_state_ = cfg_.seed ? cfg_.seed : 0x9E3779B9u;
  spawnFood();
  applySnakeToGrid();
}

State Engine::state() const { return state_; }

bool Engine::isOpposite(Direction a, Direction b) const {
  return (a == Direction::kLeft && b == Direction::kRight) ||
         (a == Direction::kRight && b == Direction::kLeft) ||
         (a == Direction::kUp && b == Direction::kDown) ||
         (a == Direction::kDown && b == Direction::kUp);
}

void Engine::resetGrid() {
  std::fill(grid_.begin(), grid_.end(), Cell::kEmpty);
}

void Engine::placeInitialSnake() {
  int cx = W() / 2, cy = H() / 2;
  snake_.clear();
  snake_.push_front({cx, cy});
  snake_.push_back({cx - 1, cy});
  snake_.push_back({cx - 2, cy});
  dir_ = Direction::kRight;
  state_ = State::kInit;
}

void Engine::applySnakeToGrid() {
  resetGrid();
  for (auto& p : snake_) grid_[idx(p.x, p.y)] = Cell::kSnake;

  if (food_.first >= 0 && food_.second >= 0)
    grid_[idx(food_.first, food_.second)] = Cell::kFood;
}

void Engine::step() {
  Point head = snake_.front();
  Point next = head;
  if (dir_ == Direction::kRight) next.x += 1;
  if (dir_ == Direction::kLeft) next.x -= 1;
  if (dir_ == Direction::kDown) next.y += 1;
  if (dir_ == Direction::kUp) next.y -= 1;

  if (next.x < 0 || next.x >= W() || next.y < 0 || next.y >= H()) {
    state_ = State::kGameOver;
    UpdateBest();
    return;
  }

  for (auto& p : snake_) {
    if (p.x == next.x && p.y == next.y) {
      state_ = State::kGameOver;
      UpdateBest();
      return;
    }
  }

  snake_.push_front(next);

  if (food_.first == next.x && food_.second == next.y) {
    score_ += 1;
    recomputeLevelAndSpeed();
    spawnFood();
  } else {
    snake_.pop_back();
  }
}

void Engine::dispatch(Event e) {
  switch (e) {
    case Event::kStart:
      if (state_ == State::kInit || state_ == State::kReady ||
          state_ == State::kGameOver) {
        placeInitialSnake();
        score_ = 0;
        recomputeLevelAndSpeed();
        applySnakeToGrid();
        state_ = State::kRunning;
      }
      break;
    case Event::kReset:
      placeInitialSnake();
      score_ = 0;
      level_ = 1;
      recomputeLevelAndSpeed();
      applySnakeToGrid();
      state_ = State::kReady;  // Готовность к запуску
      break;
    case Event::kPauseToggle:
      if (state_ == State::kRunning)
        state_ = State::kPaused;
      else if (state_ == State::kPaused)
        state_ = State::kRunning;
      break;
    case Event::kMoveUp:
    case Event::kMoveDown:
    case Event::kMoveLeft:
    case Event::kMoveRight: {
      if (state_ != State::kRunning) break;
      Direction want = dir_;
      switch (e) {
        case Event::kMoveUp:
          want = Direction::kUp;
          break;
        case Event::kMoveDown:
          want = Direction::kDown;
          break;
        case Event::kMoveLeft:
          want = Direction::kLeft;
          break;
        case Event::kMoveRight:
          want = Direction::kRight;
          break;
        default:
          break;
      }
      if (!isOpposite(dir_, want)) {
        dir_ = want;
      }
      break;
    }
    case Event::kTick:
      if (state_ == State::kRunning) {
        step();
        applySnakeToGrid();
      }
      break;
    case Event::kQuit:
      break;
  }
}

Snapshot Engine::snapshot() const {
  Snapshot s;
  s.state = state_;
  s.width = W();
  s.height = H();
  s.grid = grid_;
  s.score = score_;
  s.best = best_;
  s.level = level_;
  s.speed_ms = speed_ms_;
  s.snake.reserve(snake_.size());
  for (auto& p : snake_) s.snake.emplace_back(p.x, p.y);
  s.food = food_;
  return s;
}

bool Engine::isSnakeCell(int x, int y) const {
  for (auto& p : snake_)
    if (p.x == x && p.y == y) return true;
  return false;
}

unsigned Engine::nextRand() {
  unsigned x = rng_state_;
  x ^= x << 13;
  x ^= x >> 17;
  x ^= x << 5;
  rng_state_ = x;
  return x;
}

void Engine::spawnFood() {
  for (int tries = 0; tries < 1000; ++tries) {
    int x = (int)(nextRand() % W());
    int y = (int)(nextRand() % H());
    if (!isSnakeCell(x, y)) {
      food_ = {x, y};
      return;
    }
  }

  for (int y = 0; y < H(); ++y) {
    for (int x = 0; x < W(); ++x) {
      if (!isSnakeCell(x, y)) {
        food_ = {x, y};
        return;
      }
    }
  }

  food_ = {-1, -1};
}

std::string Engine::defaultBestPath() const {
  const char* home = std::getenv("HOME");
  std::string base = home ? home : ".";
  return base + "/.s21_snake_best";
}

int Engine::loadBestFromFile(const std::string& path) {
  std::ifstream in(path);
  int v = 0;
  if (in) in >> v;
  return v < 0 ? 0 : v;
}

void Engine::saveBestToFile(const std::string& path, int v) {
  try {
    std::filesystem::path p(path);
    if (p.has_parent_path())
      std::filesystem::create_directories(p.parent_path());
  } catch (...) {
  }

  std::ofstream out(path, std::ios::trunc);

  if (!out) return;
  out << v << "\n";
  out.flush();
}

void Engine::UpdateBest() {
  if (score_ > best_) {
    best_ = score_;
    saveBestToFile(best_path_, best_);
  }
}

int Engine::levelForScore(int score) const {
  int lvl = 1 + score / 5;
  if (lvl > 10) lvl = 10;
  return lvl;
}

int Engine::speedForLevel(int level) const {
  int base = 200;
  int step = 15;
  int ms = base - (level - 1) * step;
  return ms < 60 ? 60 : ms;
}

void Engine::recomputeLevelAndSpeed() {
  level_ = levelForScore(score_);
  speed_ms_ = speedForLevel(level_);
}
}  // namespace s21::snake