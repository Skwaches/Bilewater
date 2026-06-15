#include "fluid.h"
#include "SDL3/SDL_stdinc.h"
#include "init.h"
#include <algorithm> // Required for std::clamp

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

	//Map all 9. Don't get cheeky!
	for(int i = 0; i < gridDimensions.x; i++){
		for(int j = 0; j < gridDimensions.y; j++){
			SDL_Point groups[] = { 
				{ i-1, j-1 }, { i+0, j-1 }, { i+1, j-1 },
				{ i-1, j+0 }, { i+0, j+0 }, { i+1, j+0 },
				{ i-1, j+1 }, { i+0, j+1 }, { i+1, j+1 }
			};
			for(auto cellId: groups){
				if(cellId.x >= 0 && cellId.x < gridDimensions.x &&
				   cellId.y >= 0 && cellId.y < gridDimensions.y ){
					gridMappings[i][j].push_back(cellId);
				}
			}
		}
	}
}

void Particle::update(float time){
	velocity += gravity * time;
	velocity += gradient * time; //Gradient is directly acceleration
	position += velocity * time;
}

inline float lerp(float a, float b, float t) {
    return a + t * (b - a);
}

void Fluid::draw(SDL_Renderer* renderer){
		SDL_FColor high = {0.600f, 0.900f, 1.000f, 1.0f}; // Frothy cyan
		SDL_FColor low =  {0.000f, 0.400f, 1.000f, 1.0f}; // Deep blue
		
		float minDensity = density * 0.8f;
		float maxDensity = density * 1.5f;

		SDL_FColor cilor;
		for (size_t i = 0; i < particles.size(); ++i) {
			int index = i * circleMesh.vertices.size();
			
			float t = (particles[i].density - minDensity) / (maxDensity - minDensity);
			t = std::clamp(t, 0.0f, 1.0f); // FIX 5: Use std::clamp instead of int clamp

			cilor = { 
				lerp(low.r, high.r, t),
				lerp(low.g, high.g, t),
				lerp(low.b, high.b, t),
				1.0f
			};

			for (size_t j = 0; j < circleMesh.vertices.size(); ++j) {
				Vector2& position = particles[i].position;
				vertices[index+j].position.x = circleMesh.vertices[j].position.x + position.x;
                vertices[index+j].position.y = circleMesh.vertices[j].position.y + position.y;
				vertices[index+j].color = cilor;
            }
		}
		
		SDL_RenderGeometry(
				renderer, nullptr,
				vertices.data(), vertices.size(),
				indices.data(), indices.size()
		);
};

void Fluid::updateGrid(){
	 for(auto& column: grid){
		 for(auto& cell: column)
	 		cell.clear();
	 }

	 SDL_FPoint cellSize = { 
		 WINDOW_INFO.rect.w / gridDimensions.x, 
		 WINDOW_INFO.rect.h / gridDimensions.y
	 };

	 for(int i = 0; i < particles.size(); i++){
	 	Vector2 &position = particles[i].position;
	 	SDL_Point cellId = position / cellSize;

		cellId.x = std::clamp(cellId.x, 0, static_cast<int>(gridDimensions.x) - 1);
		cellId.y = std::clamp(cellId.y, 0, static_cast<int>(gridDimensions.y) - 1);
	 	grid[cellId.x][cellId.y].push_back(i);
	 }
}

float Fluid::kernel(float distance){
	static float volume = SDL_PI_F * SDL_pow(smoothingRadius, 5) / 10.0f;

	float value = smoothingRadius - distance;
	value *= value * value;
	return value / volume;
}

Vector2 Fluid::kernelDerivative(Vector2 displacement){
	// FIXED TYPO: pow must be 5 to match kernel integration units
	static float volume = SDL_PI_F * SDL_pow(smoothingRadius, 5) / 10.0f; 
	
	float dist = displacement.magnitude();
	if (dist == 0.0f) return {0.0f, 0.0f}; 
	
	float value = smoothingRadius - dist;
	float slope = -3.0f * value * value / volume; 
	
	Vector2 direction = displacement / dist; 
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
	// Prevent negative pressure so the fluid doesn't pull itself into artificial clumps
	return std::max(0.0f, difference * pressureMultiplier);
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
		
		if(distance < smoothingRadius && distance > 0.0001f){
			 Vector2 gradW = kernelDerivative(displacement); // Points towards B
			 
			 float pressureA = A.pressure / (A.density * A.density);
			 float pressureB = B.pressure / (B.density * B.density);
			 
			 Vector2 pressureForce = gradW * (-B.mass * (pressureA + pressureB));
			 A.gradient += pressureForce;
		}
	};
	gridIteration(updateDensities);
}

void Fluid::particleCollisions(){
	auto particleCollision = [](Particle& A, Particle& B){
			A.particleCollision(B);
	};
	gridIteration(particleCollision);
}

void Fluid::focus(float force, float range, Vector2 point, float time){
	for(auto& particle: particles){
		Vector2 displacement= point - particle.position;
		float distance = displacement.magnitude();
		if(distance >= range || distance <= 0)
			continue;
		
		Vector2 direction = displacement/distance;
		particle.velocity += direction * force * time;
	}
}

void Fluid::reset(){
	for(auto &particle: particles){
		particle.density  = particle.mass * kernel(0.0f);
		particle.pressure = 0.0f;
		particle.gradient = {0.0f, 0.0f}; // We use this property to accumulate acceleration
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
