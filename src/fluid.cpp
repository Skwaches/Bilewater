#include "fluid.h"
#include "init.h"

Fluid::Fluid(const FluidConfiguration& config):
		viscosity(config.viscosity),
		density(config.density),
		pressureMultiplier(config.pressureMultiplier),
		smoothingRadius(config.smoothingRadius),

		radius(config.radius),
		friction(config.friction),
		restitution(config.restitution),

		color(config.color),
		circleMesh({0,0}, config.radius, config.accuracy, config.color),
		vertices(config.dimensions.x * config.dimensions.y * circleMesh.vertices.size(), {.position = {0,0}, .color = config.color}),

		gridDimensions(config.gridSize.x,config.gridSize.y),
		gridMappings(config.gridSize.x, std::vector< std::vector <SDL_Point> >(config.gridSize.y, std::vector<SDL_Point>())),
		grid(config.gridSize.x, std::vector< std::vector<int> >(config.gridSize.y, std::vector<int>() ))
{ 
	Vector2 cell;
	cell +=  config.spacing + radius * 2;

	int index = 0;
	Particle particle({
			.friction = friction,
			.restitution = restitution,
			.radius = radius,
			});

	for(int i = 0; i < config.dimensions.x; i++){
		for(int j = 0; j < config.dimensions.y; j++){
			Vector2 coord(i, j);
			particle.position = config.position + cell * coord;
			particles.push_back(particle);
			int offset = index * circleMesh.vertices.size();
			for(auto id: circleMesh.indices){
				indices.push_back(id + offset);
			}
			index++;
		}
	}

	//Column by column.
	//Left to right.
	//Top to bottom
	//Only values on the right and below have not been checked.
	for(int i = 0; i < gridDimensions.x;i++){
		for(int j = 0; j < gridDimensions.y;j++){
			SDL_Point groups[] = { 
				{ i+1, j-1 },
				{ i+0, j+0 }, { i+1, j+0 },
				{ i+0, j+1 }, { i+1, j+1 }};
			for(auto cellId: groups){
				if(
						cellId.x < gridDimensions.x&&
						cellId.y < gridDimensions.y&&
						cellId.y >= 0
				  ){
					gridMappings[i][j].push_back(cellId);
				}
			}
		}
	}
}

void Particle::update(float time){
	velocity += acceleration * time;
	position += velocity * time;
}

void Fluid::draw(SDL_Renderer* renderer){

		int index = 0;
		for (size_t i = 0; i < particles.size(); ++i) {
			int index = i * circleMesh.vertices.size();
			for (size_t j = 0; j < circleMesh.vertices.size(); ++j) {
				Vector2& position = particles[i].position;
				vertices[index+j].position.x = circleMesh.vertices[j].position.x + position.x;
                vertices[index+j].position.y = circleMesh.vertices[j].position.y + position.y;
            }
		}

		SDL_RenderGeometry(
				renderer, 
				nullptr,
				vertices.data(), 
				vertices.size(),
				indices.data(), 
				indices.size()
				);

};

int clamp(int x, int min, int max){
	return std::max( std::min(x, max), min);
}

void Fluid::updateGrid(){
	 //Remove outdated data
	 for(auto& column: grid){
		 for(auto& cell: column)
	 		cell.clear();
	 }

	 //Consider making the cellSize depend on the smoothing radius
	 SDL_FPoint cellSize = { 
		 WINDOW_INFO.rect.w/gridDimensions.x, 
		 WINDOW_INFO.rect.h/gridDimensions.y
	 };

	 for(int i = 0; i < particles.size(); i++){
	 	Vector2 &position = particles[i].position;
	 	SDL_Point cellId = position/cellSize;

		//Clamp just in case it escapes
		cellId.x = clamp(cellId.x, 0, static_cast<int>(gridDimensions.x) - 1);
		cellId.y = clamp(cellId.y, 0, static_cast<int>(gridDimensions.y) - 1);
	 	grid[cellId.x][cellId.y].push_back(i);
	 }
	

}

//We want 1 at 0 and 0 at smoothingRadius
float Fluid::smoothingKernel(float distance){
	float value = smoothingRadius - distance;
	return value;
}

void Fluid::updateDensities(){

}

void Fluid::update(float time){
	for(auto &particle: particles){
		particle.update(time);
		particle.containerCollision(WINDOW_INFO.rect);
	}
	updateGrid();

}
