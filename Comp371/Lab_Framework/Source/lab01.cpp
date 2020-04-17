//
// COMP 371 Labs Framework
//
// Created by Nicolas Bergeron on 20/06/2019.
//

#include "COMP371Helper.h"
#include "Model.h"
#include <vector>

using namespace glm;
using namespace std;

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
	int grassSeed = 2354583;
	int grassZoom = 1;
	double grassPersistence = 0.7;
	GLuint grassTextureID = makeNoiseTexture(grassSeed, grassZoom, grassPersistence);
#else
	int grassSeed = 2354583;
	int grassZoom = 1;
	double grassPersistence = 0.7;
	GLuint grassTextureID = makeNoiseTexture(grassSeed, grassZoom, grassPersistence);
#endif

	
	// GL_TEXTURE0 IS RESERVED FOR SHADOW MAPPING
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, grassTextureID);

	//------------------------------------Shader Programs----------------------------------------//

	// Compile and link shaders here ...
	int shaderProgramBasic = compileAndLinkShaders("Comp371Basic.vshader", "Comp371Basic.fshader");
	int shaderProgramTexture = compileAndLinkShaders("Comp371Texture.vshader", "Comp371Texture.fshader");
	int shaderProgramShadow = compileAndLinkShaders("Comp371Shadow.vshader", "Comp371Shadow.fshader");
	int shaderProgramLightSource = compileAndLinkShaders("Comp371LightSource.vshader", "Comp371LightSource.fshader");

	//-----------------------------------------VAOs--------------------------------------------//

		//Setup models
#if defined(PLATFORM_OSX)
	string spherePath = "Models/UVSphereTriangle.obj";
	string cubePath = "Models/cube.obj";
#else
	string spherePath = "../Assets/Models/UVSphereTriangle.obj";
	string cubePath = "../Assets/Models/cube.obj";
#endif

	int sphereVertices;
	GLuint vaoSphereModel = setupModelEBO(spherePath, sphereVertices);

	int cubeVertices;
	GLuint vaoCubeModel = setupModelEBO(cubePath, cubeVertices);

	int vaoCube = createVertexArrayObjectCube();
	int vaoGround = createVertexArrayObjectGround();

	//-----------------------------------------SHADOWS--------------------------------------//

	// Create framebuffer object for rendering the depth map
	GLuint depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);

	// Create a 2D texture that we'll use as the framebuffer's depth buffer
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

	GLuint depthMap;
	glGenTextures(1, &depthMap);
	glActiveTexture(GL_TEXTURE0); // Texture2 for shadow rendering because Texture1 and texture2 is for ground and nose textures
	glBindTexture(GL_TEXTURE_2D, depthMap);
	setTexture(shaderProgramBasic, "shadowMap", 0);
	setTexture(shaderProgramTexture, "shadowMap", 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	// Attach the generated depth texture as the framebuffer's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//----------------------------------------Camera------------------------------------------//

	// Camera parameters for view transform
	vec3 cameraPosition(0.6f, 10.0f, 1.0f);
	vec3 cameraLookAt(0.0f, 0.0f, -1.0f);
	vec3 cameraUp(0.0f, 1.0f, 0.0f);
	vec3 cameraTarget = vec3(0.0f, 0.0f, 0.0f);
	vec3 cameraDirection = normalize(cameraPosition - cameraTarget);

	// THE FOLLOWING IS IMPORTANT FOR THE LIGHTING
	setVec3(shaderProgramBasic, "viewPos", cameraPosition);
	setVec3(shaderProgramTexture, "viewPos", cameraPosition);

	// Other camera parameters
	float cameraSpeed = 1.0f;
	float cameraFastSpeed = 2 * cameraSpeed;
	float cameraHorizontalAngle = 90.0f;
	float cameraVerticalAngle = 0.0f;

	//-------------------------------------ProjectionMatrix---------------------------------------//

	// Set projection matrix
	mat4 projectionMatrix = glm::perspective(70.0f,            // field of view in degrees
		800.0f / 600.0f,  // aspect ratio
		0.01f, 800.0f);   // near and far (near > 0)


	setMat4(shaderProgramBasic, "projectionMatrix", projectionMatrix);
	setMat4(shaderProgramTexture, "projectionMatrix", projectionMatrix);
	setMat4(shaderProgramLightSource, "projectionMatrix", projectionMatrix);

	//---------------------------------------ViewMatrix-------------------------------------------//

	// Set initial view matrix
	mat4 viewMatrix = lookAt(cameraPosition,  // eye
		 cameraLookAt,  // center
		cameraUp); // up

	setMat4(shaderProgramBasic, "viewMatrix", viewMatrix);
	setMat4(shaderProgramTexture, "viewMatrix", viewMatrix);
	setMat4(shaderProgramLightSource, "viewMatrix", viewMatrix);

	//-----------------------------------Initial Mouse Position---------------------------------//
	
	// For frame time
	float lastFrameTime = glfwGetTime();
	int lastMouseLeftState = GLFW_RELEASE;
	double lastMousePosX, lastMousePosY;
	glfwGetCursorPos(window, &lastMousePosX, &lastMousePosY);

	//-----------------------------------------Settings?-----------------------------------------//

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	//---------------------------------------Miscellaneous------------------------------------//

	// Put future variables here

	//-----------------------------------------Fog ------------------------------------------//

	int fogEnabled = 0; //int controlling fog switch

	setInt(shaderProgramBasic, "fogEnabled", fogEnabled);
	setInt(shaderProgramTexture, "fogEnabled", fogEnabled);


	// This is probably how we'll need to initiate objects, in a vector of 'Model's
	Cube cube1(vec3(0.0f, 0.0f, 0.0f), vec3(0.4f, 0.4f, 0.4f));
	Cube cube2(vec3(5.0f, 0.0f, -20.0f), vec3(1.0f, 1.0f, 1.0f));
	vector<Model> models;
	/*models.push_back(cube1);
	models.push_back(cube2);*/

	//----------------------------------------------------------------------------------------//

	// Entering Game Loop
	while (!glfwWindowShouldClose(window))
	{
		// Frame time calculation
		float dt = glfwGetTime() - lastFrameTime;
		lastFrameTime += dt;
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//-----------------------------------------SHADOWS--------------------------------------//

		vec3 lightPos = vec3(0.001f, 200.0f, -150.0f);
		mat4 lightProjectionMatrix = ortho(-100.0f, 100.0f, -100.0f, 100.0f, 1.0f, 400.0f);
		mat4 lightViewMatrix = lookAt(lightPos, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
		mat4 lightSpaceMatrix = lightProjectionMatrix * lightViewMatrix;

		setVec3(shaderProgramBasic, "lightPos", lightPos);
		setVec3(shaderProgramTexture, "lightPos", lightPos);
		setMat4(shaderProgramBasic, "lightSpaceMatrix", lightSpaceMatrix);
		setMat4(shaderProgramTexture, "lightSpaceMatrix", lightSpaceMatrix);

		// Cull the front side for shadow mapping, then cull backside again after
		glCullFace(GL_FRONT);

		// 1. first render to depth map
		setMat4(shaderProgramShadow, "lightSpaceMatrix", lightSpaceMatrix);
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		//----------------------------------Draw ground shadow----------------------------------//
		glBindVertexArray(vaoGround);
		glUseProgram(shaderProgramShadow);

		mat4 groundWorldMatrix = translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f)) *
			scale(mat4(1.0f), vec3(50.0f, 1.0f, 50.0f)); // 100 * 100 grid now
		setMat4(shaderProgramShadow, "worldMatrix", groundWorldMatrix);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// Cull backside again for the actual rendering
		glCullFace(GL_BACK);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//--------------------------------NOW DO ACTUAL RENDERING-------------------------------//

		const unsigned int SCR_WIDTH = 1024, SCR_HEIGHT = 768;
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//----------------------------------Draw light source-----------------------------------//
		glBindVertexArray(vaoSphereModel);
		mat4 lightSourceMatrix = translate(mat4(1.0), lightPos) * scale(mat4(1.0), vec3(10.0f, 10.0f, 10.0f));
		setMat4(shaderProgramLightSource, "worldMatrix", lightSourceMatrix);
		glDrawElements(GL_TRIANGLES, sphereVertices, GL_UNSIGNED_INT, 0);

		//----------------------------------Draw ground-----------------------------------------//
		glBindVertexArray(vaoGround);
		glUseProgram(shaderProgramTexture);

		groundWorldMatrix = translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f)) * 
			scale(mat4(1.0f), vec3(50.0f, 1.0f, 50.0f)); // 100 * 100 grid now
		setMat4(shaderProgramTexture, "worldMatrix", groundWorldMatrix);
		setVec3(shaderProgramTexture, "aColor", vec3(1.0f, 1.0f, 1.0f));

		// The following is to make the grass texture repeat so that it doesn't become blurry
		setFloat(shaderProgramTexture, "uvMultiplier", 12.0f);
		// Activate texture1 where the grass texture is located
		glActiveTexture(GL_TEXTURE0 + 1);
		GLuint textureLocation = glGetUniformLocation(shaderProgramTexture, "textureSampler");
		glUniform1i(textureLocation, 1);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		setFloat(shaderProgramTexture, "uvMultiplier", 1.0f);
		setVec3(shaderProgramTexture, "aColor", vec3(1.0f, 1.0f, 1.0f));

		//-----------------------------Draw Test Cubes for collision----------------------------//

		/*glBindVertexArray(vaoCubeModel);
		mat4 worldMatrix;

		for (int i = 0; i < models.size(); i++) {
			worldMatrix = translate(mat4(1.0f), models[i].translationVector) * 
				scale(mat4(1.0f), models[i].scaleVector);
			setMat4(shaderProgramBasic, "worldMatrix", worldMatrix);
			glDrawElements(GL_TRIANGLES, cubeVertices, GL_UNSIGNED_INT, 0);
		}*/

		//----------------------------------------------------------------------------------------//

		glBindVertexArray(0);

		// End Frame
		glfwSwapBuffers(window);
		glfwPollEvents();

		//--------------------------------------Handle Inputs-------------------------------------//

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

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
		cameraVerticalAngle = std::max(-85.0f, std::min(85.0f, cameraVerticalAngle));
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
		

		if(glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) // regenerate grass
		{
			grassSeed = rand() % (3000000 - 2000000 + 1) + 2000000;
			grassZoom = rand() % (12 - 1 + 1) + 1;
			grassTextureID = makeNoiseTexture(grassSeed, grassZoom, grassPersistence);
			glBindTexture(GL_TEXTURE_2D, grassTextureID);
		}

		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) // move camera to the left
		{
			cameraPosition -= cameraSideVector * currentCameraSpeed * dt;

			for (int i = 0; i < models.size(); i++) {
				if (checkCollision(cameraPosition, models[i].box)) {
					cameraPosition += cameraSideVector * currentCameraSpeed * dt;
					break;
				}
			}
		}

		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) // move camera to the right
		{
			cameraPosition += cameraSideVector * currentCameraSpeed * dt;

			for (int i = 0; i < models.size(); i++) {
				if (checkCollision(cameraPosition, models[i].box)) {
					cameraPosition -= cameraSideVector * currentCameraSpeed * dt;
					break;
				}
			}
		}

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) // move camera forward
		{
			cameraPosition += cameraLookAt * currentCameraSpeed * dt;
			cameraPosition.y = std::max(0.1f, cameraPosition.y); // Make sure it doesn't go below ground

			for (int i = 0; i < models.size(); i++) {
				if (checkCollision(cameraPosition, models[i].box)) {
					cameraPosition -= cameraLookAt * currentCameraSpeed * dt;
					break;
				}
			}
		}

		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) // move camera backward
		{
			cameraPosition -= cameraLookAt * currentCameraSpeed * dt;
			cameraPosition.y = std::max(0.1f, cameraPosition.y); // Make sure it doesn't go below ground

			for (int i = 0; i < models.size(); i++) {
				if (checkCollision(cameraPosition, models[i].box)) {
					cameraPosition += cameraLookAt * currentCameraSpeed * dt;
					break;
				}
			}
		}


		if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) //toggle Fog effect on and off
		{
			if (fogEnabled == 0) {
				fogEnabled = 1;
				setInt(shaderProgramBasic, "fogEnabled", fogEnabled);
				setInt(shaderProgramTexture, "fogEnabled", fogEnabled);
			}
			else
			{
				fogEnabled = 0;
				setInt(shaderProgramBasic, "fogEnabled", fogEnabled);
				setInt(shaderProgramTexture, "fogEnabled", fogEnabled);
			}
		}



		setVec3(shaderProgramBasic, "viewPos", cameraPosition);
		setVec3(shaderProgramTexture, "viewPos", cameraPosition);

		mat4 viewMatrix = lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);

		setMat4(shaderProgramBasic, "viewMatrix", viewMatrix);
		setMat4(shaderProgramTexture, "viewMatrix", viewMatrix);
		setMat4(shaderProgramLightSource, "viewMatrix", viewMatrix);
		glUseProgram(shaderProgramBasic);

	}


	// Shutdown GLFW
	glfwTerminate();

	return 0;
}

