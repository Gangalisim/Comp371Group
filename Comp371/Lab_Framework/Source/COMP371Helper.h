#pragma once

#include <iostream>
#include <list>
#include <algorithm>

#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler

#include <GLFW/glfw3.h> // cross-platform interface for creating a graphical context,
// initializing OpenGL and binding inputs

#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices
#include <glm/common.hpp>
#include <ctime> 
#include <fstream>
#include <string>

using namespace glm;
using namespace std;

extern GLFWwindow* window;

struct BoundingBox {
	BoundingBox(vec3 _position, float _x, float _y, float _z) : position(_position), sizeX(_x), sizeY(_y), sizeZ(_z) {}
	BoundingBox() : position(vec3(0.0f, 0.0f, 0.0f)), sizeX(5.0f), sizeY(5.0f), sizeZ(5.0f) {};

	vec3 position;
	float sizeX; // The horizontal (x-axis) length from center to edge of box
	float sizeY; // The vertical length from center to edge of box
	float sizeZ; // The horizontal (z-axis) length from center to edge of box
};

bool initContext();
int compileAndLinkShaders(string vertexPath, string fragmentPath);
int createVertexArrayObjectCube();
int createVertexArrayObjectGround();
int createVertexArrayObjectParticles();
GLuint loadTexture(const char* filename);
GLuint makeNoiseTexture(int seed, int zoom, double persistence, float lightColor[3], float darkColor[3]);
GLuint setupModelEBO(string path, int& vertexCount);

void setMat4(int shaderProgram, const GLchar* location, mat4 matrix);
void setVec3(int shaderProgram, const GLchar* location, vec3 vector);
void setFloat(int shaderProgram, const GLchar* location, float value);
void setTexture(int shaderProgram, const GLchar* location, int value);
void setInt(int shaderProgram, const GLchar* location, int value);

bool checkCollision(vec3 camera, BoundingBox b);
int getCurrentQuadrant(vec3 position);