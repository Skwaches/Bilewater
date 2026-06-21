#pragma once
#include "vector2.hpp"
#include <vector>

struct FluidConfiguration{
		int points = 40;

		float target_Density = 30;
		float viscosity = 20,
			  pressureMultiplier = 200,
			  smoothingRadius = 4,
			  radius = 4;
		int meshPoints = 20;

		bool random = false;

		Vector2 spacing = 2;
		Vector2 gravity = 30;
		SDL_FRect region = {0, 0, 400, 400};  

		SDL_FColor high_Color;		
		SDL_FColor low_Color;		
};

void Circle_Generation(
		std::vector<SDL_Vertex> &vertices,
		std::vector<int> &indices,

		float radius = 5,
		int point_num = 12, 

		SDL_FColor center_Color = {0.800,0.200,0.702, 1},
		SDL_FColor outer_Color =  {0.800,0.200,0.702, 1},

		Vector2 center = {0,0}
		);

class Fluid{
	public:
		SDL_FColor high_Color;		
		SDL_FColor low_Color;		

		std::vector<Vector2> positions;
		std::vector<Vector2> velocities;
		std::vector<Vector2> densities;
				            
		std::vector<SDL_Vertex> vertices;
		std::vector<int> indices;

		std::vector<SDL_Vertex> mesh_Vertices;
		std::vector<int> mesh_Indices;


		Vector2 gravity;
		float target_Density,
			  viscosity,
			  pressureMultiplier,
			  smoothingRadius,
			  radius;

		Fluid(const FluidConfiguration config);
		void drawCPU(SDL_Renderer* renderer);
		void drawGPU(SDL_Renderer* renderer);
};

