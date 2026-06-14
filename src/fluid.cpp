#include "fluid.h"
#include "init.h"

Fluid::Fluid(Vector2 dimensions, float radius, Vector2 position, Vector2 spacing, int accuracy, SDL_FColor color):
	radius(radius), color(color), circleMesh({0,0}, radius, accuracy, color){

		Vector2 cell;
		cell +=  spacing + radius * 2;

		int index = 0;
		Particle particle;
		for(int i = 0; i < dimensions.x; i++)
			for(int j = 0; j < dimensions.y; j++){
				Vector2 coord(i, j);
				Vector2 center = position + cell * coord;
				particle.position = center;
				particles.push_back(particle);
				index++;
			}
}

void Fluid::draw(SDL_Renderer* renderer){

        std::vector<SDL_Vertex> renderBuffer = circleMesh.vertices;
        for (const Particle& p : particles) {
            // 1. Offset 
            for (size_t i = 0; i < renderBuffer.size(); ++i) {
                renderBuffer[i].position.x = circleMesh.vertices[i].position.x + p.position.x;
                renderBuffer[i].position.y = circleMesh.vertices[i].position.y + p.position.y;
            }
            SDL_RenderGeometry(
                renderer, 
                nullptr,
                renderBuffer.data(), 
                renderBuffer.size(),
                circleMesh.indices.data(), 
                circleMesh.indices.size()
            );
	}
};

void particleCollisions(Particle A, Particle B){
	
}


void wallCollisions(Particle& particle, float dampingFactor = 0.6){
    // Left Wall
    if(particle.position.x <= 0) {
        particle.position.x = 0; 
        particle.velocity.x *= -dampingFactor;
    }
    // Right Wall
    else if(particle.position.x >= WINDOW_INFO.size.x) {
        particle.position.x = WINDOW_INFO.size.x;
        particle.velocity.x *= -dampingFactor;
    }

    // Top Wall
    if(particle.position.y <= 0) {
        particle.position.y = 0; 
        particle.velocity.y *= -dampingFactor;
    }
    // Bottom Wall
    else if(particle.position.y >= WINDOW_INFO.size.y) {
        particle.position.y = WINDOW_INFO.size.y;
        particle.velocity.y *= -dampingFactor;
    }
}


void Fluid::collisions(float dampingFactor){
	auto wallColl = [dampingFactor](Particle& particle){wallCollisions(particle,dampingFactor);};
	this->pass(wallColl);
}

void Fluid::update(float time){
	auto integrate = [time](Particle& particle){ 
		particle.velocity += particle.acceleration * time;
		Vector2 displacement = particle.velocity * time;
		particle.position += displacement;
	};
	this->pass(integrate);
}
