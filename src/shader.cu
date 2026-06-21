#include "shader.hpp"

#include "init.hpp"
#define CUDA_CHECK(expression)\
	do {\
			cudaError_t success = expression;\
			if(success != cudaSuccess){\
				APP_STATE = SDL_APP_FAILURE;\
				SDL_Log( "CUDA Runtime Error!\nFile: %s\nLine %i\nError: %s\n",\
						__FILE__,__LINE__, cudaGetErrorString(success));\
						}\
		} while(0)

Vector2 *CUDA_positions = nullptr,
		 *CUDA_velocities = nullptr; 

SDL_Vertex *CUDA_vertices = nullptr;
SDL_Vertex *CUDA_meshVertices = nullptr;

size_t pointSize;
size_t verticesSize;
int mesh_size;

void kernel_Init( int num_points, std::vector<SDL_Vertex> mesh_vertices){
	int num_vertices = mesh_vertices.size() * num_points;

	pointSize = sizeof(Vector2) * num_points;
	mesh_size = mesh_vertices.size();
	size_t meshSize = sizeof(SDL_Vertex) * mesh_size;
	verticesSize = meshSize * num_points;

	CUDA_CHECK( cudaMalloc((void**) &CUDA_positions, pointSize) );
	CUDA_CHECK( cudaMalloc((void**) &CUDA_velocities, pointSize) );

	CUDA_CHECK( cudaMalloc((void**) &CUDA_meshVertices, meshSize));
	CUDA_CHECK( cudaMalloc((void**) &CUDA_vertices, verticesSize)); 

	CUDA_CHECK( cudaMemcpy(CUDA_meshVertices, mesh_vertices.data(), meshSize, cudaMemcpyHostToDevice) );
}

__global__ void render_mesh(
		Vector2* positions, SDL_Vertex* vertices, 
		SDL_Vertex* mesh_vertices = CUDA_meshVertices,
		int esh_size = mesh_size){
	int workingIndex =  blockDim.x * blockIdx.x + threadIdx.x;

	for(int i = 0; i < esh_size; i++){
    	vertices[workingIndex].position.x = mesh_vertices[i].position.x + positions[workingIndex].x;
    	vertices[workingIndex].position.y = mesh_vertices[i].position.y + positions[workingIndex].y;
	}

}

void kernel_Draw(std::vector<Vector2> &positions, std::vector<SDL_Vertex> &vertices){
	
	//Copy data to GPU
	CUDA_CHECK( cudaMemcpy(CUDA_positions, positions.data(), pointSize, cudaMemcpyHostToDevice) );
	CUDA_CHECK( cudaMemcpy(CUDA_vertices, vertices.data(), verticesSize, cudaMemcpyHostToDevice) );

	//Call Kernel
	int threadsPerBlock = 256;
	int blocks = (threadsPerBlock + positions.size())/threadsPerBlock;
	render_mesh<<<blocks,threadsPerBlock>>>( CUDA_positions, CUDA_vertices );
	CUDA_CHECK(cudaGetLastError());
	CUDA_CHECK(cudaDeviceSynchronize());
	
	//Copy results
	CUDA_CHECK( cudaMemcpy(vertices.data(), CUDA_vertices, verticesSize, cudaMemcpyDeviceToHost) );
}



void kernel_Close(){
	cudaFree(CUDA_positions);
	cudaFree(CUDA_velocities);
	cudaFree(CUDA_vertices);
}
