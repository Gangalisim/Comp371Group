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
	vec3 pos(0.0f, 0.0f, 0.0f);
	box = BoundingBox(pos, 5.0f, 5.0f, 5.0f);
}

Tree::Tree(vec3 translation, vec3 scale)
{
	translationVector = translation;
	scaleVector = scale;
	vec3 pos(0.0f, 0.0f, 0.0f);
	box = BoundingBox(vec3(pos.x + translation.x, pos.y + translation.y, pos.z + translation.z),
		5.0f * scale.x, 5.0f * scale.y, 5.0f * scale.z);
}
