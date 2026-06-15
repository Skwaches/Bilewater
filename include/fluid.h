#pragma once
#include "circle.h"
#include "vector2.h"

typedef struct Particle{
	float pressure = 0.0;
	Vector2    position , 
			   velocity = {-60, 0}, 
			   acceleration = {6, 30};
}Particle;

typedef struct FluidConfiguration{
				float radius = 4.0f;
				float density = 1.0f;
				float friction = 0.01;
				float restitution = 0.99f;

				Vector2 position = {40.0f,40.0f};
				Vector2 spacing= {40,40};

				SDL_Point dimensions = {15,15};
				SDL_Point gridSize = {20, 20};

				int accuracy= 100;
				SDL_FColor color = {0.30, 0.1, 0.79, 1};//rgba(51, 230, 179, 1) ;
}FluidConfiguration;

class Fluid{
	public:
		std::vector<Particle> particles;
		SDL_FColor color;		
		float radius ,
			  viscosity,
			  density ,
			  friction ,
			  restitution;
			            
		Circle circleMesh;
		std::vector<int> indices;
		std::vector<SDL_Vertex> vertices;
		
		//This is an attempt to optimise collision checking
		Vector2 gridDimensions;
		std::vector< std::vector< std::vector <int>>> grid; 
		std::vector< std::vector< std::vector <SDL_Point> > > gridMappings;

		
		Fluid(const FluidConfiguration& config);
		
		void draw(SDL_Renderer* renderer);
		void collisions();
		void gridCollisions();
		void update(float time);

		//Pass all the particles through a function and do nothing.
		template<typename Func>
			void pass(Func fn){
				for(auto& particle:particles)
						fn(particle);
			}

};

