#include "fluid.h"
#include "SDL3/SDL_stdinc.h"
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
	Vector2 cell = config.spacing + radius * 2;

	int index = 0;
	Particle particle({
			.friction = friction,
			.restitution = restitution,
			.radius = radius,
			});

	Vector2 offset;
	for(int i = 0; i < config.dimensions.x; i++){
		for(int j = 0; j < config.dimensions.y; j++){
			if(config.random)
				offset = {config.range.x * SDL_randf(), config.range.y * SDL_randf()};
			else{
				Vector2 coord(i, j);
				offset = cell * coord;
			}

			particle.position = config.position + offset;
			particles.push_back(particle);
			int indexOffset = index * circleMesh.vertices.size();
			for(auto id: circleMesh.indices){
				indices.push_back(id + indexOffset);
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
	Vector2 pressureAcceleration = density == 0.0f ? 0.0f: gradient/-density; //Move away from the high pressure.
	velocity += acceleration * time;
	velocity += pressureAcceleration * time;
	position += velocity * time;
}

int clamp(float x, float min, float max){
	return std::max( std::min(x, max), min);
}

inline float lerp(float a, float b, float t) {
    return a + t * (b - a);
}

void Fluid::draw(SDL_Renderer* renderer){
		int index = 0;
		SDL_FColor high = {0.900f,0.100f,0.300f,1.0f};
		SDL_FColor low =  {0.100f,0.200f,0.800f,1.0f};
		SDL_FPoint range = {density * 10.0f, density * 0.8f};

		SDL_FColor cilor;
		for (size_t i = 0; i < particles.size(); ++i) {
			int index = i * circleMesh.vertices.size();
			float t = (particles[i].density-range.y)/ (range.x - range.y);
			SDL_Log("%f",particles[i].density);
			t = clamp(t, 0.0f,1.0f);
			cilor = { 
				lerp(low.r, high.r, t),
				lerp(low.g, high.g, t),
				lerp(low.b, high.b, t),
				1.0f};

			for (size_t j = 0; j < circleMesh.vertices.size(); ++j) {
				Vector2& position = particles[i].position;
				vertices[index+j].position.x = circleMesh.vertices[j].position.x + position.x;
                vertices[index+j].position.y = circleMesh.vertices[j].position.y + position.y;
				vertices[index+j].color = cilor;
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

float Fluid::kernel(float distance){
	//One fluid at a time please!
	static float volume = SDL_PI_F * SDL_pow(smoothingRadius, 5)/10.0f;

	float value = smoothingRadius - distance;
	value *= value*value;
	return value/volume;
}
Vector2 Fluid::kernelDerivative(Vector2 displacement){
	static float volume = SDL_PI_F * SDL_pow(smoothingRadius, 4) / 10.0f; 
	
	float dist = displacement.magnitude();
	if (dist == 0.0f) return {0.0f, 0.0f}; // Prevent division by zero
	
	float value = smoothingRadius - dist;
	float slope = -3.0f * value * value / volume; // Scalar derivative
	
	Vector2 direction = displacement / dist; // Normalized direction
	return direction * slope;
}
void Fluid::updateDensity(){
	auto updateDensities = [this](Particle& A, Particle& B){
		float distance = (A.position - B.position).magnitude();
		if(distance < smoothingRadius){
			A.density += B.mass * kernel(distance);
		}
	};
	gridIteration(updateDensities);
}
float densityToPressure(float Targetdensity, float density, float pressureMultiplier){
	float difference = density - Targetdensity;
	return difference * pressureMultiplier;
}

void Fluid::updatePressure(){
	for(auto& particle: particles){
		particle.pressure = densityToPressure(density, particle.density, pressureMultiplier);
	}
}

void Fluid::updateGradient(){
	auto updateDensities = [this](Particle& A, Particle& B){
		Vector2 displacement = (A.position - B.position);
		float distance = displacement.magnitude();
		if(distance < smoothingRadius){
			 A.gradient += 
				 kernelDerivative(displacement) *  
				 densityToPressure(density, B.density, pressureMultiplier)
				 * B.mass/B.density;
		}
	};
	gridIteration(updateDensities);
}

void Fluid::particleCollisions(){
	auto particleCollision = [](Particle& A, Particle& B){A.particleCollision(B);};
	gridIteration(particleCollision);
}

void Fluid::reset(){
	for(auto &particle: particles){
		particle.density  = 0;
		particle.gradient = {0.0f, 0.0f};
	}

}

void Fluid::update(float time){
	updateGrid();

	reset();

	particleCollisions();
	updateDensity();
	updatePressure();
	updateGradient();
	for(auto &particle: particles){
		particle.update(time);
		particle.containerCollision(WINDOW_INFO.rect);
	}
}
