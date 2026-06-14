#include "fluid.h"
#include "init.h"

Fluid::Fluid(
		Vector2 dimensions, float radius,
		Vector2 position, Vector2 spacing, 
		int accuracy, SDL_FColor color,
		SDL_Point gridSize):
	radius(radius), color(color),
	circleMesh({0,0}, radius, accuracy, color),
	gridDimensions(gridSize.x,gridSize.y),
	grid(gridSize.x, std::vector< std::vector<int> >(gridSize.y, std::vector<int>() )), 
	gridMappings(gridSize.x, std::vector< std::vector <SDL_Point> >(gridSize.y, std::vector<SDL_Point>())),
	vertices(dimensions.x * dimensions.y * circleMesh.vertices.size(), {.position = {0,0}, .color = color})
{ 

	Vector2 cell;
	cell +=  spacing + radius * 2;

	int index = 0;
	Particle particle;
	for(int i = 0; i < dimensions.x; i++){
		for(int j = 0; j < dimensions.y; j++){
			Vector2 coord(i, j);
			particle.position = position + cell * coord;
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

void particleCollision(Particle& A, Particle& B, float radius, float dampingFactor=0.6){
	Vector2 displacement = A.position - B.position;
	if(displacement.x == 0.0f && displacement.y != 0.0f){
		float jitter = (SDL_randf() - 0.5f) * 0.2f;
		displacement.x += jitter;
	}
	float distance = displacement.magnitude();
	
	if(distance == 0.0f){
		displacement = {
			(SDL_randf() - 0.5f),
			(SDL_randf() - 0.5f)
		};
		distance = displacement.magnitude();
	}
	float difference = distance - radius * 2;
	if( difference < 0 ){
		Vector2 normal = displacement/distance;
		
		Vector2 pushVector = normal * (-difference/2.0f);
		A.position += pushVector;
		B.position -= pushVector;
		
		Vector2 relative_Velocity = A.velocity - B.velocity;
		float normalComponent = relative_Velocity.dot(normal);

		float j = -(1.0f + dampingFactor) * (normalComponent/2.0f);

		if(normalComponent > 0)
			return;

		Vector2 impulse = normal * j;
		A.velocity += impulse;
		B.velocity -= impulse;
	}
}

void Fluid::naiveCollisions(float dampingFactor){
	for(size_t i = 0; i < particles.size() - 1;i++){
		for(size_t j = i + 1; j < particles.size();j++){
			particleCollision(particles[i],particles[j], radius, dampingFactor);
		}
	}
}

void Fluid::gridCollisions(float dampingFactor){
	for(int i = 0; i < gridDimensions.x;i++)
		for(int j = 0; j < gridDimensions.y;j++)
			for(auto& A_id: grid[i][j])
				for (auto& neighbors: gridMappings[i][j])
					for(auto& B_id: grid[neighbors.x][neighbors.y]){
						if(A_id == B_id)
							continue;
						particleCollision(particles[A_id], particles[B_id], radius,dampingFactor);
					}
}

void wallCollisions(Particle& particle, float radius, float dampingFactor = 1){
    // Left Wall
    if(particle.position.x <= radius) {
        particle.position.x = radius; 
        particle.velocity.x *= -dampingFactor;
    }
    // Right Wall
    else if(particle.position.x + radius >= WINDOW_INFO.size.x) {
        particle.position.x = WINDOW_INFO.size.x - radius;
        particle.velocity.x *= -dampingFactor;
    }

    // Top Wall
    if(particle.position.y <= radius) {
        particle.position.y = radius; 
        particle.velocity.y *= -dampingFactor;
    }
    // Bottom Wall
    else if(particle.position.y + radius >= WINDOW_INFO.size.y) {
        particle.position.y = WINDOW_INFO.size.y - radius;
        particle.velocity.y *= -dampingFactor;
    }
}

void Fluid::collisions(float dampingFactor){
	auto wallColl = [this,dampingFactor](Particle& particle){
		wallCollisions(particle,radius, dampingFactor);
	};
	pass(wallColl);
	gridCollisions(dampingFactor);
	//naiveCollisions(dampingFactor);

}

int clamp(int x, int min, int max){
	return std::max( std::min(x, max), min);
}

void Fluid::update(float time){
	 //Remove outdated data
	 for(auto& column: grid){
		 for(auto& cell: column)
	 		cell.clear();
	 }

	 SDL_FPoint cellSize = { 
		 WINDOW_INFO.size.x/gridDimensions.x, 
		 WINDOW_INFO.size.y/gridDimensions.y};

	 for(int i = 0; i < particles.size(); i++){
	 	Vector2 &position = particles[i].position;
	 	SDL_Point cellId = position/cellSize;

		//Clamp just in case it escapes
		cellId.x = clamp(cellId.x, 0, static_cast<int>(gridDimensions.x) - 1);
		cellId.y = clamp(cellId.y, 0, static_cast<int>(gridDimensions.y) - 1);
	 	grid[cellId.x][cellId.y].push_back(i);
	 }
	
	auto integrate = [time](Particle& particle){ 
		particle.velocity += particle.acceleration * time;
		Vector2 displacement = particle.velocity * time;
		particle.position += displacement;
	};
	this->pass(integrate);
}
