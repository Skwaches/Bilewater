#include "vector2.hpp"
#include <vector>
#include "fluid.hpp"

//Circle is counted among the point_num. It is the first item in the vector
void Circle_Generation(
		std::vector<SDL_Vertex> &vertices,
		std::vector<int> &indices,

		float radius,
		int point_num,

		SDL_FColor center_Color,
		SDL_FColor outer_Color,

		Vector2 center
		){

	SDL_Vertex vertex = { .position = center, .color = center_Color};
	vertices.push_back(vertex);
	vertex.color = outer_Color;

	static const float revolution = SDL_PI_F * 2.0f; 

	point_num = std::max(4, point_num-1);
	float delta = revolution / point_num ;
	float angle = 0.0f;

	for(int i = 0; i < point_num; ++i){
		vertex.position = { 
			center.x + radius * SDL_cosf(angle), 
			center.y + radius * SDL_sinf(angle) 
		};
		vertices.push_back(vertex);
		angle += delta;
	}

	//Generate indices
	int index = 0;
	while(index < point_num ){
		indices.push_back(0);
		indices.push_back( index + 1 );

		//Jump over 0 after looping!
		(index == point_num - 1)?  indices.push_back(1): indices.push_back(index + 2); 

		index++;
	}
}
