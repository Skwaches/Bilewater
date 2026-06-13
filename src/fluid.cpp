#include "fluid.h"
#include "SDL3/SDL_render.h"
#include "init.h"

Fluid::Fluid(SDL_Point dimensions, float radius, SDL_FPoint position, SDL_FPoint spacing, int accuracy, SDL_FColor color):
	radius(radius), color(color){

	SDL_FPoint cell = {
		radius * 2 + spacing.x, 
		radius * 2 + spacing.y};
	
	int index = 0;
	Particle particle;
	for(int i = 0; i < dimensions.x; i++)
		for(int j = 1; j < dimensions.y; j++){
			SDL_FPoint center = {
				position.x +  cell.x * i, 
				position.y +  cell.y * j
			};
			particle.circle = Circle(center, radius, accuracy, color);
			particle.position = center;
			particles.push_back(particle);
			index++;
		}
}

void Fluid::draw(SDL_Renderer* renderer){
   for (Particle& particle : particles) {
	   Circle& circle = particle.circle;
	   SDL_CHECK(SDL_RenderGeometry(
			   renderer, nullptr,
			   circle.vertices.data(),
			   circle.vertices.size(),
			   circle.indices.data(),
			   circle.indices.size()));
   };
	return;
}


Circle::Circle(SDL_FPoint center, float radius, int accuracy, SDL_FColor color):
	radius(radius),
	center(center)
{
	accuracy = std::max(4,accuracy);

	SDL_Vertex vertex = {.position = center, .color = color, .tex_coord = {0,0} };
	vertices.push_back(vertex);
	float revolution = SDL_PI_F * 2.0f; 
	float delta = revolution / accuracy ;
	float angle = 0.0f;

	while(angle < revolution){ 
		vertex.position = { center.x + radius * SDL_cosf(angle), center.y + radius * SDL_sinf(angle) };
		vertices.push_back(vertex);
		angle += delta;
	}
	
	//Generate indices
	//Jesus Christ I'm smart
	int index = 0;
	while(index < accuracy ){
		indices.push_back(0);
		indices.push_back( index + 1 );

		if( index == accuracy - 1)
			indices.push_back(1); 
		else
			indices.push_back(index + 2); 
			
		index++;
	}
}

