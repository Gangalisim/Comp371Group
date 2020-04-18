#pragma once
#include "COMP371Helper.h"

class Particle {
public:

	Particle();

	bool expired();
	void update(float dt, vec3 currentLookAt);

	vec3 position;
	int velocity;
	vec3 billboardRotationAxis;
	float billboardRotationAngle;

	static vec3 originalAxis; // This is for billboard stuff

};