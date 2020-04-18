#include "Particles.h"

vec3 Particle::originalAxis(0, 0, 1);

Particle::Particle()
{
	int x, y, z;
	x = (rand() % 101); // Generate number from 0 to 100
	x -= 50; // Make its range from -50 to 50
	z = (rand() % 101);
	z -= 50;
	y = (rand() % 26); // Generate the initial height from 0 to 25

	position = initialPosition = vec3(x, y, z);
	velocity = (rand() % 2) + 1; // Three sets of speed -> 1, 2, or 3
	billboardRotationAxis = vec3(0, 1, 0);
	billboardRotationAngle = 0.0f;
	directionAge = 0.0f;
	xDirection = (rand() % 2);
	zDirection = (rand() % 2);
}

bool Particle::expired()
{
	return (position.y <= 0); // If the particle has hit the ground
}

void Particle::update(float dt, vec3 cameraPosition)
{
	this->position.y -= dt * velocity;
	if (this->expired()) {
		this->position = vec3(initialPosition.x, 25.0f, initialPosition.z);
	}

	//----------------------------------------------------//

	this->position.x = xDirection ? this->position.x + dt : this->position.x - dt;
	this->position.z = zDirection ? this->position.z + dt : this->position.z - dt;
	if (directionAge >= DIR_LIFETIME) {
		xDirection = 1 - xDirection;
		zDirection = 1 - zDirection;
	}

	//------------------------------------------------------//

	cameraPosition = normalize(cameraPosition - this->position);
	vec3 cameraLookAt(-cameraPosition);
	cameraLookAt.y = 0; //project onto xz plane
	cameraLookAt.x *= -1;
	cameraLookAt = normalize(cameraLookAt);

	billboardRotationAngle = -glm::acos(-1 * dot(Particle::originalAxis, cameraLookAt)) *
		360 / (2 * pi<float>());

	if (cameraLookAt.x > 0) {
		billboardRotationAngle = 360 - billboardRotationAngle;
	}
}
