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

bool initContext();
int compileAndLinkShaders(string vertexPath, string fragmentPath);
int createVertexArrayObjectCube();
int createVertexArrayObjectGround();
GLuint loadTexture(const char* filename);
GLuint setupModelEBO(string path, int& vertexCount);

void setMat4(int shaderProgram, const GLchar* location, mat4 matrix);
void setVec3(int shaderProgram, const GLchar* location, vec3 vector);
void setFloat(int shaderProgram, const GLchar* location, float value);
void setTexture(int shaderProgram, const GLchar* location, int value);