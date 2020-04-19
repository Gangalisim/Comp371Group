#include "Model.h"

Cube::Cube()
{
	translationVector = vec3(0.0f, 0.0f, 0.0f);
	scaleVector = vec3(1.0f, 1.0f, 1.0f);
	vec3 pos(0.0f, 0.0f, 0.0f);
	box = BoundingBox(pos, 5.0f, 5.0f, 5.0f);
	// The 5.0f size value is hardcoded because that was the value 
	// of the size based on Cube.obj (the pos also)
}

Cube::Cube(vec3 translation, vec3 scale)
{
	translationVector = translation;
	scaleVector = scale;
	vec3 pos(0.0f, 0.0f, 0.0f);
	box = BoundingBox(vec3(pos.x + translation.x, pos.y + translation.y, pos.z + translation.z),
		5.0f * scale.x, 5.0f * scale.y, 5.0f * scale.z);
}

Tree::Tree()
{
	translationVector = vec3(0.0f, 0.0f, 0.0f);
	scaleVector = vec3(1.0f, 1.0f, 1.0f);
	//vec3 pos(-0.1483415f, 4.9745075f, -0.0472805f);
	vec3 pos(-0.1483415f, 0.0f, -0.0472805f);
	box = BoundingBox(pos, 0.8139465f, 10.140205f, 0.7113755f);
}

Tree::Tree(vec3 translation, vec3 scale)
{
	translationVector = translation;
	scaleVector = scale;
	//vec3 pos(-0.1483415f, 4.9745075f, -0.0472805f);
	vec3 pos(-0.1483415f, 0.0f, -0.0472805f);
	box = BoundingBox(vec3(pos.x + translation.x, pos.y + translation.y, pos.z + translation.z),
		0.8139465f * scale.x, 10.140205f * scale.y, 0.7113755f * scale.z);
}

Wolf::Wolf()
{
	translationVector = vec3(0.0f, 3.5f, 0.0f);
	scaleVector = vec3(1.0f, 1.0f, 1.0f);
	vec3 pos(0.0f, -0.5744805f, 0.0039735f);
	box = BoundingBox(pos, 1.082057f, 2.9391915f, 4.0801215f);
}

Wolf::Wolf(vec3 translation, vec3 scale)
{
	translationVector = translation;
	scaleVector = scale;
	vec3 pos(0.0f, -0.5744805f, 0.0039735f);
	box = BoundingBox(vec3(pos.x + translation.x, pos.y + translation.y, pos.z + translation.z),
		1.082057f * scale.x, 2.9391915f * scale.y, 4.0801215f * scale.z);
}
