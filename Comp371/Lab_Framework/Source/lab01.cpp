//
// COMP 371 Labs Framework
//
// Created by Nicolas Bergeron on 20/06/2019.
//

#include "COMP371Helper.h"

using namespace glm;
using namespace std;

int createVertexArrayObject();

int main(int argc, char*argv[])
{
	if (!initContext()) return -1;

	// Disable mouse cursor
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Black background
	glClearColor(135.0f/255.0f, 206.0f/255.0f, 235.0f/255.0f, 1.0f);

	//---------------------------------------Textures--------------------------------------------//
	// Load Textures
#if defined(PLATFORM_OSX)
	GLuint grassTextureID = loadTexture("Textures/grass.jpg");
#else
	GLuint grassTextureID = loadTexture("../Assets/Textures/grass.jpg");
#endif

	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, grassTextureID);

	//------------------------------------Shader Programs----------------------------------------//

	// Compile and link shaders here ...
	int shaderProgram = compileAndLinkShaders("Comp371Basic.vshader", "Comp371Basic.fshader");
	int shaderProgramTexture = compileAndLinkShaders("Comp371Texture.vshader", "Comp371Texture.fshader");


	//----------------------------------------Camera------------------------------------------//

	// Camera parameters for view transform
	vec3 cameraPosition(0.6f, 1.0f, 10.0f);
	vec3 cameraLookAt(0.0f, 0.0f, -1.0f);
	vec3 cameraUp(0.0f, 1.0f, 0.0f);
	vec3 cameraTarget = vec3(0.0f, 0.0f, 0.0f);
	vec3 cameraDirection = normalize(cameraPosition - cameraTarget);

	// Other camera parameters
	float cameraSpeed = 1.0f;
	float cameraFastSpeed = 2 * cameraSpeed;
	float cameraHorizontalAngle = 90.0f;
	float cameraVerticalAngle = 0.0f;
	bool  cameraFirstPerson = true; // press 1 or 2 to toggle this variable

									// Spinning cube at camera position
	float spinningCubeAngle = 0.0f;

	//-------------------------------------ProjectionMatrix---------------------------------------//

	// Set projection matrix
	mat4 projectionMatrix = glm::perspective(70.0f,            // field of view in degrees
		800.0f / 600.0f,  // aspect ratio
		0.01f, 100.0f);   // near and far (near > 0)

	glUseProgram(shaderProgram);
	GLuint projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");
	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
	glUseProgram(shaderProgramTexture);
	projectionMatrixLocation = glGetUniformLocation(shaderProgramTexture, "projectionMatrix");
	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

	//---------------------------------------ViewMatrix-------------------------------------------//

	// Set initial view matrix
	mat4 viewMatrix = lookAt(cameraPosition,  // eye
		 cameraLookAt,  // center
		cameraUp); // up

	glUseProgram(shaderProgram);
	GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
	glUseProgram(shaderProgramTexture);
	viewMatrixLocation = glGetUniformLocation(shaderProgramTexture, "viewMatrix");
	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);

	//-----------------------------------------VAOs--------------------------------------------//

	// Define and upload geometry to the GPU here ...
	int vao = createVertexArrayObject();
	int vaoGround = createVertexArrayObjectGround();

	//----------------------------------------------------------------------------------------//
	// For frame time
	float lastFrameTime = glfwGetTime();
	int lastMouseLeftState = GLFW_RELEASE;
	double lastMousePosX, lastMousePosY;
	glfwGetCursorPos(window, &lastMousePosX, &lastMousePosY);

	//-----------------------------------------Settings?-----------------------------------------//

	// Other OpenGL states to set once before the Game Loop
	// Enable Backface culling
	glEnable(GL_CULL_FACE);

	glEnable(GL_DEPTH_TEST); // @TODO 1 - Enable Depth Test
							 // ...
	glDepthMask(GL_TRUE);

	//---------------------------------------Miscellaneous------------------------------------//

	// Put future variables here

	//----------------------------------------------------------------------------------------//

	// Entering Game Loop
	while (!glfwWindowShouldClose(window))
	{
		// Frame time calculation
		float dt = glfwGetTime() - lastFrameTime;
		lastFrameTime += dt;
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//----------------------------------------------------------------------------------------//

		// Draw ground
		glBindVertexArray(vaoGround);
		glUseProgram(shaderProgramTexture);

		mat4 groundWorldMatrix = translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f)) * scale(mat4(1.0f), vec3(50.0f, 0.0f, 50.0f));
		GLuint worldMatrixLocation = glGetUniformLocation(shaderProgramTexture, "worldMatrix");
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &groundWorldMatrix[0][0]);

		// The following is to make the grass texture repeat so that it doesn't become blurry
		worldMatrixLocation = glGetUniformLocation(shaderProgramTexture, "uvMultiplier");
		glUniform1f(worldMatrixLocation, 12.0f);

		// Activate texture1 where the grass texture is located
		glActiveTexture(GL_TEXTURE0 + 1);
		GLuint textureLocation = glGetUniformLocation(shaderProgramTexture, "textureSampler");
		glUniform1i(textureLocation, 1);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		//----------------------------------------------------------------------------------------//

		//glUseProgram(shaderProgram);
		//glBindVertexArray(vao);
		//{
		//	// In third person view, let's draw the spinning cube in world space, like any other models
		//	mat4 spinningCubeWorldMatrix = translate(mat4(1.0f), cameraPosition) *
		//		rotate(mat4(1.0f), radians(spinningCubeAngle), vec3(0.0f, 1.0f, 0.0f)) *
		//		scale(mat4(1.0f), vec3(2.0f, 2.0f, 2.0f));

		//	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &spinningCubeWorldMatrix[0][0]);
		//}
		//glDrawArrays(GL_TRIANGLES, 0, 36);


		glBindVertexArray(0);

		// End Frame
		glfwSwapBuffers(window);
		glfwPollEvents();

		//--------------------------------------Handle Inputs-------------------------------------//

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		// This was solution for Lab02 - Moving camera exercise
		// We'll change this to be a first or third person camera
		bool fastCam = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
		float currentCameraSpeed = (fastCam) ? cameraFastSpeed : cameraSpeed;
		
		double mousePosX, mousePosY;
		glfwGetCursorPos(window, &mousePosX, &mousePosY);

		double dx = mousePosX - lastMousePosX;
		double dy = mousePosY - lastMousePosY;

		lastMousePosX = mousePosX;
		lastMousePosY = mousePosY;

		// Convert to spherical coordinates
		const float cameraAngularSpeed = 60.0f;
		cameraHorizontalAngle -= dx * cameraAngularSpeed * dt;
		cameraVerticalAngle   -= dy * cameraAngularSpeed * dt;

		// Clamp vertical angle to [-85, 85] degrees
		//cameraVerticalAngle = std::max(-85.0f, std::min(85.0f, cameraVerticalAngle));
		if (cameraHorizontalAngle > 360)
		{
		cameraHorizontalAngle -= 360;
		}
		else if (cameraHorizontalAngle < -360)
		{
		cameraHorizontalAngle += 360;
		}

		float theta = radians(cameraHorizontalAngle);
		float phi = radians(cameraVerticalAngle);

		cameraLookAt = vec3(cosf(phi)*cosf(theta), sinf(phi), -cosf(phi)*sinf(theta));
		vec3 cameraSideVector = glm::cross(cameraLookAt, vec3(0.0f, 1.0f, 0.0f));

		glm::normalize(cameraSideVector);
		

		// @TODO 5 = use camera lookat and side vectors to update positions with ASDW
		// adjust code below
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) // move camera to the left
		{
			cameraPosition -= cameraSideVector * currentCameraSpeed * dt;
		}

		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) // move camera to the right
		{
			cameraPosition += cameraSideVector * currentCameraSpeed * dt;
		}

		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) // move camera up
		{
			cameraPosition -= cameraLookAt * currentCameraSpeed * dt;
		}

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) // move camera down
		{
			cameraPosition += cameraLookAt * currentCameraSpeed * dt;
		}
		mat4 viewMatrix = lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);

		glUseProgram(shaderProgram);
		GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
		glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
		glUseProgram(shaderProgramTexture);
		viewMatrixLocation = glGetUniformLocation(shaderProgramTexture, "viewMatrix");
		glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);

	}


	// Shutdown GLFW
	glfwTerminate();

	return 0;
}

int createVertexArrayObject()
{
	// Cube model
	vec3 vertexArray[] = {  // position,                            color
		vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f), //left - red
		vec3(-0.5f,-0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f),
		vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f),

		vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f),
		vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f),
		vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f),

		vec3(0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f), // far - blue
		vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f),
		vec3(-0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f),

		vec3(0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f),
		vec3(0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f),
		vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f),

		vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 1.0f), // bottom - turquoise
		vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 1.0f, 1.0f),
		vec3(0.5f,-0.5f,-0.5f), vec3(0.0f, 1.0f, 1.0f),

		vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 1.0f),
		vec3(-0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 1.0f),
		vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 1.0f, 1.0f),

		vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), // near - green
		vec3(-0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),
		vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),

		vec3(0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),
		vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),
		vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),

		vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 1.0f), // right - purple
		vec3(0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 1.0f),
		vec3(0.5f, 0.5f,-0.5f), vec3(1.0f, 0.0f, 1.0f),

		vec3(0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 1.0f),
		vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 1.0f),
		vec3(0.5f,-0.5f, 0.5f), vec3(1.0f, 0.0f, 1.0f),

		vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 0.0f), // top - yellow
		vec3(0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 0.0f),
		vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 0.0f),

		vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 0.0f),
		vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 0.0f),
		vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 0.0f)
	};


	// Create a vertex array
	GLuint vertexArrayObject;
	glGenVertexArrays(1, &vertexArrayObject);
	glBindVertexArray(vertexArrayObject);


	// Upload Vertex Buffer to the GPU, keep a reference to it (vertexBufferObject)
	GLuint vertexBufferObject;
	glGenBuffers(1, &vertexBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArray), vertexArray, GL_STATIC_DRAW);

	glVertexAttribPointer(0,                   // attribute 0 matches aPos in Vertex Shader
		3,                   // size
		GL_FLOAT,            // type
		GL_FALSE,            // normalized?
		2 * sizeof(vec3), // stride - each vertex contain 2 vec3 (position, color)
		(void*)0             // array buffer offset
	);
	glEnableVertexAttribArray(0);


	glVertexAttribPointer(1,                            // attribute 1 matches aColor in Vertex Shader
		3,
		GL_FLOAT,
		GL_FALSE,
		2 * sizeof(vec3),
		(void*)sizeof(vec3)      // color is offseted a vec3 (comes after position)
	);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glBindVertexArray(0);

	return vertexArrayObject;
}

