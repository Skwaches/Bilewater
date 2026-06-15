#pragma once
#include "circle.h"
#include "vector2.h"

typedef struct ParticleConfiguration{
		float pressure = 0.0,
			  density = 0.0,
			  friction = 0.01f,
			  restitution = 0.99f,
			  mass = 1.0f,
			  radius = 3;

		Vector2    position = {0, 0}, 
				   velocity = {0, 0}, 
				   acceleration = {0, 10},
				   gradient = {0,0};
		;
				   
}ParticleConfiguration;

class Particle{
	public:
		float pressure,
			  density,
			  friction,
			  restitution,
			  mass,
			  radius;

		Vector2    position, 
				   velocity, 
				   acceleration,
				   gradient;

		Particle(ParticleConfiguration config):
		pressure(config.pressure),
		density(config.density),
		friction(config.friction),
		restitution(config.restitution),
		mass(config.mass),
		radius(config.radius),
		position(config.position),
		velocity(config.velocity),
		acceleration(config.acceleration),
		gradient(config.gradient){}

		void update(float time);
		void surfaceCollision(Vector2 normal, float offset);
		void containerCollision(SDL_FRect Rect);
		void particleCollision(Particle& other);
};

typedef struct FluidConfiguration{
				//Fluid particle details
				float viscosity = 4.0f;
				float density = 1.0f;
				float pressureMultiplier = 20.0f;
				float smoothingRadius = 10.0f;
				
				//Particle collisions details
				float friction = 0.01;
				float restitution = 0.99f;
				float radius = 4.0f;

				//Initialisation conditions
				bool random = true;
				Vector2 range = {400, 500};
				Vector2 position = {40.0f,40.0f};
				Vector2 spacing= {40,40};

				SDL_Point dimensions = {15,15};
				SDL_Point gridSize = {20, 20};
				int accuracy= 100;
				SDL_FColor color = {207.0f/255, 51.0f/255, 176.0f/255.0f, 1};//rgba(207, 51, 176, 1)
}FluidConfiguration;

class Fluid{
	public:
		std::vector<Particle> particles;
		SDL_FColor color;		
		float radius ,
			  viscosity,
			  density ,
			  friction,
			  restitution,
			  pressureMultiplier,
			  smoothingRadius;
			            
		Circle circleMesh;
		std::vector<int> indices;
		std::vector<SDL_Vertex> vertices;
		Vector2 gridDimensions;
		std::vector< std::vector< std::vector <int>>> grid; 
		std::vector< std::vector< std::vector <SDL_Point> > > gridMappings;

		//Holy Dog Water
		template<typename Func>
			void gridIteration(Func fn){
				for(int i = 0; i < gridDimensions.x;i++)
					for(int j = 0; j < gridDimensions.y;j++)
						for(auto& A_id: grid[i][j])
							for (auto& neighbors: gridMappings[i][j])
								for(auto& B_id: grid[neighbors.x][neighbors.y]){
									if(A_id == B_id)
										continue;
									fn(particles[A_id], particles[B_id]);
								}
			}

		
		Fluid(const FluidConfiguration& config);
		
		void reset();
		void draw(SDL_Renderer* renderer);
		void update(float time);
		void particleCollisions();

		float kernel(float distance);
		Vector2 kernelDerivative(Vector2 distance);
		void updateDensity();
		void updatePressure();
		void updateGradient();
		void updateGrid(); 
};

