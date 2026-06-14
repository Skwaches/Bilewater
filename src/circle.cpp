#include "circle.h"
#include "init.h"

Circle::Circle(Vector2 center, float radius, int accuracy, SDL_FColor color):
	radius(radius),
	center(center)
{
	accuracy = std::max(4,accuracy);

	SDL_Vertex vertex = {.position = center, .color = color, .tex_coord = {0,0} };
	vertices.push_back(vertex);
	float revolution = SDL_PI_F * 2.0f; 
	float delta = revolution / accuracy ;
	float angle = 0.0f;

	for(int i = 0; i < accuracy; ++i){
		vertex.position = { center.x + radius * SDL_cosf(angle), center.y + radius * SDL_sinf(angle) };
		vertices.push_back(vertex);
		angle += delta;
	}
	
	//Generate indices
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

void Circle::draw(SDL_Renderer* renderer){
	   SDL_CHECK(SDL_RenderGeometry(
			   renderer, nullptr,
			   vertices.data(),
			   vertices.size(),
			   indices.data(),
			   indices.size()));
}
