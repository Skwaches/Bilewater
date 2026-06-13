#pragma once
#include <SDL3/SDL.h>
#include <vector>

class Circle{
	public:
		SDL_FPoint center = {0,0};
		float radius = 0; 
		std::vector<SDL_Vertex> vertices;
		std::vector<int> indices;
		Circle(SDL_FPoint center={0,0}, float radius= 10, int accuracy=50, SDL_FColor color = {0.3,0.56,0.45, 1});
};

typedef struct Particle{
	float pressure = 0.0;
	SDL_FPoint position = {0,0}, 
			   velocity = {0,0}, 
			   acceleration = {0,0};
	Circle circle = Circle();
}Particle;

class Fluid{
	public:
		std::vector<Particle> particles;
		SDL_FColor color;
		float radius = 2
			, viscosity = 1
			, density = 1;

		Fluid(
				SDL_Point dimensions = {10,10}, float radius = 10, SDL_FPoint position = {20,20}, 
				SDL_FPoint spacing= {6,7}, int accuracy= 50, SDL_FColor color = {1.30, 1.1, 1.79, 1});
		void draw(SDL_Renderer* renderer);
};

