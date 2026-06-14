#pragma once
#include "circle.h"
#include "vector2.h"

typedef struct Particle{
	float pressure = 0.0;
	Vector2    position , 
			   velocity = {-60, 0}, 
			   acceleration = {6, 30};
}Particle;

class Fluid{
	public:
		std::vector<Particle> particles;
		SDL_FColor color;
		float radius = 2
			, viscosity = 1
			, density = 1;
		Circle circleMesh;
		std::vector<int> indices;
		std::vector<SDL_Vertex> vertices;
		
		//This is an attempt to optimise collision checking
		Vector2 gridDimensions;
		std::vector< std::vector< std::vector <int>>> grid; 
		std::vector< std::vector< std::vector <SDL_Point> > > gridMappings;

		
		Fluid(  
				Vector2 dimensions = {15,15}, float radius = 4, 
				Vector2 position = {40,40}, Vector2 spacing= {40,40},
				int accuracy= 100, SDL_FColor color = {0.30, 0.1, 0.79, 1}
				,SDL_Point gridSize = {20, 20}
				);

		void draw(SDL_Renderer* renderer);
		void collisions(float dampingFactor = 0.6);
		void naiveCollisions(float dampingFactor);
		void gridCollisions(float dampingFactor);
		void update(float time);

		//Pass all the particles through a function and do nothing.
		template<typename Func>
			void pass(Func fn){
				for(auto& particle:particles)
						fn(particle);
			}

};

