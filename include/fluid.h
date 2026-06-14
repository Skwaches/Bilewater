#pragma once
#include "circle.h"
#include "vector2.h"

typedef struct Particle{
	float pressure = 0.0;
	Vector2    position , 
			   velocity , 
			   acceleration = {0, 30};
}Particle;

class Fluid{
	public:
		std::vector<Particle> particles;
		SDL_FColor color;
		float radius = 2
			, viscosity = 1
			, density = 1;
		int size;
		Circle circleMesh;
		Fluid(
				Vector2 dimensions = {10,5}, float radius = 10, Vector2 position = {20,20}, 
				Vector2 spacing= {6,7}, int accuracy= 100, SDL_FColor color = {0.30, 0.1, 0.79, 1});
		void draw(SDL_Renderer* renderer);
		void collisions(float dampingFactor = 0.6);
		void update(float time);

		//Pass all the particles through a function and do nothing.
		template<typename Func>
			void pass(Func fn){
				for(auto& particle:particles)
						fn(particle);
			}

};

