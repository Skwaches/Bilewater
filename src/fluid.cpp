#include "fluid.h"
#include "init.h"

Fluid::Fluid(const FluidConfiguration& config):
		radius(config.radius),
		density(config.density),
		friction(config.friction),
		restitution(config.restitution),
		color(config.color),
		circleMesh({0,0}, config.radius, config.accuracy, config.color),
		gridDimensions(config.gridSize.x,config.gridSize.y),
		grid(config.gridSize.x, std::vector< std::vector<int> >(config.gridSize.y, std::vector<int>() )), 
		gridMappings(config.gridSize.x, std::vector< std::vector <SDL_Point> >(config.gridSize.y, std::vector<SDL_Point>())),
		vertices(config.dimensions.x * config.dimensions.y * circleMesh.vertices.size(), {.position = {0,0}, .color = config.color})
		{ 
			Vector2 cell;
			cell +=  config.spacing + radius * 2;

			int index = 0;
	Particle particle;
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

void particleCollision(
		Particle& A, Particle& B, 
		float radius, float restitution=0.6, 
		float friction=0.1){
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
		Vector2 tangent = { normal.y, -normal.x };
		
		Vector2 pushVector = normal * (-difference/2.0f);
		A.position += pushVector;
		B.position -= pushVector;
		
		Vector2 relative_Velocity = A.velocity - B.velocity;

		float normalComponent = relative_Velocity.dot(normal);
		float tangentComponent = relative_Velocity.dot(tangent);

		//Sliding Friction
		Vector2 impulse =  tangent * ( tangentComponent/2.0f) * friction;

		//Elastic Collision
		impulse += (normal * normalComponent) * restitution;

		if(normalComponent > 0)
			return;

		A.velocity -= impulse;
		B.velocity += impulse;
	}
}

void Fluid::gridCollisions(){
	for(int i = 0; i < gridDimensions.x;i++)
		for(int j = 0; j < gridDimensions.y;j++)
			for(auto& A_id: grid[i][j])
				for (auto& neighbors: gridMappings[i][j])
					for(auto& B_id: grid[neighbors.x][neighbors.y]){
						if(A_id == B_id)
							continue;
						particleCollision(
								particles[A_id], particles[B_id], 
								radius,restitution, friction);
					}
}

//NOTE! The value of offset must be relative to the direction of the normal
//Normal must be a unit vector
void wallCollision(
		Particle& particle, float radius, 
		Vector2 normal, float offset, 
		float restitution, float friction){

	Vector2 tangent = { normal.y, -normal.x};
	float distance = particle.position.dot(normal) - offset;
	float normalComponent = particle.velocity.dot(normal); //Speed in the direction of the wall
	float overlap = distance - radius;

	if(normalComponent < 0 && overlap <= 0) {
		float newNormalComponent = -normalComponent * restitution;

		float tangentComponent = particle.velocity.dot(tangent);
		float newTangentComponent = tangentComponent * (1-friction);

		particle.velocity = (normal * newNormalComponent) + (tangent * newTangentComponent );
		particle.position -= normal * overlap; }
}

void windowEdge_Collisions(
		Particle& particle, float radius,
		float restitution = 1, float friction = 0.01){
	auto wallBound = [&particle,radius,restitution,friction](Vector2 normal,float offset){
		wallCollision(particle, radius, normal, offset, restitution, friction);
	};

	// Left wall
	wallBound({1,0},0);
	
    // Top Wall
	wallBound({0,1},0);
	
	//Right Wall
	wallBound( {-1,0}, -static_cast<float>(WINDOW_INFO.size.x));

	//Bottom Wall
	wallBound( {0,-1}, -static_cast<float>(WINDOW_INFO.size.y));
}

void Fluid::collisions(){

	auto wallBound = [this](Particle& particle){
		windowEdge_Collisions(particle, radius, restitution, friction);
	};
	pass(wallBound);
	gridCollisions();
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
