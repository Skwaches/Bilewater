#pragma once
#include <SDL3/SDL.h>
#include <vector>
#include "vector2.hpp"


void kernel_Init( int num_points, std::vector<SDL_Vertex> mesh_vertices);
void kernel_Draw(std::vector<Vector2> &positions, std::vector<SDL_Vertex> &vertices);
void kernel_Close();
