#pragma once
#include "vector2.h"
#include <SDL3/SDL.h>
#include <vector>

class Circle {
public:
  Vector2 center = {0, 0};
  float radius = 0;
  std::vector<SDL_Vertex> vertices;
  std::vector<int> indices;
  Circle(Vector2 center = {0, 0}, float radius = 10, int accuracy = 50,
         SDL_FColor color = {0.3, 0.56, 0.45, 1});
  void draw(SDL_Renderer *renderer);
  template <typename Func> void pass(Func fn) {
    for (auto &vertex : vertices)
      fn(vertex);
  }
};
