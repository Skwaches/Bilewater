//I got a bit lost...
#include "fluid.h"

//NOTE! The value of offset must be relative to the direction of the normal
//Normal must be a unit vector
void Particle::surfaceCollision(Vector2 normal, float offset){ 
	Vector2 tangent = { normal.y, -normal.x};
	float distance = position.dot(normal) - offset;
	float normalComponent = velocity.dot(normal); //Speed in the direction of the wall
	float overlap = distance - radius;

	if(normalComponent <= 0 && overlap <= 0) {
		float newNormalComponent = -normalComponent * restitution;

		float tangentComponent = velocity.dot(tangent);
		float newTangentComponent = tangentComponent * (1-friction);

		velocity = (normal * newNormalComponent) + (tangent * newTangentComponent );
		position -= normal * overlap; }
}


void Particle::containerCollision(SDL_FRect container){
	// Left wall
	float offset = container.x;
	surfaceCollision({1,0},0);
	
    // Top Wall
	offset = container.y;
	surfaceCollision({0,1},0);
	
	//Right Wall
	offset = -(container.x + container.w); 
	surfaceCollision( {-1,0}, offset);

	//Bottom Wall
	offset = -(container.y + container.h);
	surfaceCollision( {0,-1}, offset);
}

void Particle::particleCollision(Particle& other){
	Vector2 displacement = position - other.position;
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
		position += pushVector;
		other.position -= pushVector;
		
		Vector2 relative_Velocity = velocity - other.velocity;

		float normalComponent = relative_Velocity.dot(normal);
		float tangentComponent = relative_Velocity.dot(tangent);

		//Sliding Friction
		Vector2 impulse =  tangent * ( tangentComponent/2.0f) * friction;

		//Elastic Collision
		impulse += (normal * normalComponent) * restitution;

		if(normalComponent > 0)
			return;

		velocity -= impulse;
		other.velocity += impulse;
	}
}
