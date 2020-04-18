#pragma once
#include "COMP371Helper.h"

class Particle {
public:

	Particle();

	bool expired();
	void update(float dt, vec3 currentLookAt);

	vec3 position;
	vec3 initialPosition; // 'position' resets to 'initialPosition' after hitting the ground
	int velocity;
	vec3 billboardRotationAxis;
	float billboardRotationAngle;

	// The particle will fall in one direction for 5 seconds continuously, before changing dir
	const float DIR_LIFETIME = 5;
	int directionAge; // Direction age
	int xDirection;
	int zDirection;

	static vec3 originalAxis; // This is for billboard stuff

};