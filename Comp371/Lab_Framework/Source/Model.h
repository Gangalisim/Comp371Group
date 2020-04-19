#pragma once
#include "COMP371Helper.h"

class Model
{
public:

	BoundingBox box;
	vec3 translationVector;
	vec3 scaleVector;
};

class Cube : public Model
{
public:

	Cube();
	Cube(vec3 translation, vec3 scaleVector);

};

class Tree : public Model
{
public:
	// Class tree will need to be modified based on the .obj file that we choose
	Tree();
	Tree(vec3 translation, vec3 scaleVector);

};

class Wolf : public Model
{
public:
	// Class tree will need to be modified based on the .obj file that we choose
	Wolf();
	Wolf(vec3 translation, vec3 scaleVector);

};