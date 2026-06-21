#include "fluid.hpp"
#include "shader.hpp"

Fluid::Fluid(const FluidConfiguration config):
		viscosity(config.viscosity),
		target_Density(config.target_Density),
		pressureMultiplier(config.pressureMultiplier),
		smoothingRadius(config.smoothingRadius),
		radius(config.radius),
		gravity(config.gravity),
		positions(config.points),
		velocities(config.points),
		densities(config.points)
{ 
	Circle_Generation(mesh_Vertices, mesh_Indices, config.radius, config.meshPoints);
	vertices.resize(mesh_Vertices.size() * config.points, mesh_Vertices[1] );

	const Vector2 cell = config.spacing + config.radius * 2;

	Vector2 size(config.region.w,config.region.h);
	Vector2 position(config.region.x,config.region.y);
	position += config.radius;
	size -= config.radius;

	const Vector2 center = size/2.0f + position;

	const int dimensions = SDL_sqrtf(config.points); //Number of cells each side.
	const Vector2 topLeft = center - cell * dimensions/2.0f;

	Vector2 offset;
	for(int i = 0; i < config.points ; i++){
		if(config.random){
			positions[i]= {
				size.x * SDL_randf(),
				size.y * SDL_randf()};
			positions[i] += position;
		}
		else{
			int y = i/dimensions;
			int x = i%dimensions;
			Vector2 coord(x,y);
			offset = coord * cell;
			positions[i] = offset + topLeft;
		}
		SDL_Log("%f, %f", positions[i].x,positions[i].y);

		int indexOffset = i * mesh_Vertices.size();
		for(int j = 0; j< mesh_Indices.size(); j++)
			indices.push_back(mesh_Indices[j] + indexOffset);
	}
}

void Fluid::drawCPU(SDL_Renderer* renderer){
		for (size_t i = 0; i < positions.size(); ++i) {
			int index = i * mesh_Vertices.size();
			Vector2& position = positions[i];
			for (size_t j = 0; j < mesh_Vertices.size(); ++j) {
				vertices[index+j].position.x = mesh_Vertices[j].position.x + position.x;
                vertices[index+j].position.y = mesh_Vertices[j].position.y + position.y;
            }
		}
		SDL_RenderGeometry(
				renderer, nullptr,
				vertices.data(), vertices.size(),
				indices.data(), indices.size()
		);
};

void Fluid::drawGPU(SDL_Renderer* renderer){
	kernel_Draw(positions, vertices);
	SDL_RenderGeometry(
			renderer, nullptr,
			vertices.data(), vertices.size(),
			indices.data(), indices.size()
			);

}
