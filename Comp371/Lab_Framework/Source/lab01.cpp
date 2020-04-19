
#include "COMP371Helper.h"
#include "Model.h"
#include <vector>
#include "Particles.h"

#include <irrKlang.h>
using namespace irrklang;

using namespace glm;
using namespace std;

int main(int argc, char*argv[])
{
	if (!initContext()) return -1;

	srand((unsigned)time(0));

	// Disable mouse cursor
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Black background
	glClearColor(135.0f/255.0f, 206.0f/255.0f, 235.0f/255.0f, 1.0f);

	//---------------------------------------Textures--------------------------------------------//
	// Load Textures
	float lightTextureColor[3] = { 255. / 255., 255. / 255., 255. / 255. };
	float darkTextureColor[3] = { 147. / 255.,231. / 255., 251 / 255. };
#if defined(PLATFORM_OSX)
	int grassSeed = 2354583;
	int grassZoom = 1;
	double grassPersistence = 0.7;
	GLuint grassTextureID = makeNoiseTexture(grassSeed, grassZoom, grassPersistence, lightTextureColor, darkTextureColor);
	GLuint snowflakeTextureID = loadTexture("Textures/particle.png");
	GLuint trunkTextureID = loadTexture("Textures/trunk.jpg");
	GLuint wolfTextureID = loadTexture("Textures/marble.jpg");
#else
	int grassSeed = 2354583;
	int grassZoom = 1;
	double grassPersistence = 0.7;
	GLuint grassTextureID = makeNoiseTexture(grassSeed, grassZoom, grassPersistence, lightTextureColor, darkTextureColor);
	GLuint snowflakeTextureID = loadTexture("../Assets/Textures/particle.png");
	GLuint trunkTextureID = loadTexture("../Assets/Textures/trunk.jpg");
	GLuint marbleTextureID = loadTexture("../Assets/Textures/marble.jpg");
#endif

	
	// GL_TEXTURE0 IS RESERVED FOR SHADOW MAPPING
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, grassTextureID);
	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, snowflakeTextureID);
	// GL_TEXTURE0 + 3 was the old tree texture that was removed
	glActiveTexture(GL_TEXTURE0 + 4);
	glBindTexture(GL_TEXTURE_2D, trunkTextureID);
	glActiveTexture(GL_TEXTURE0 + 5);
	glBindTexture(GL_TEXTURE_2D, marbleTextureID);

	//------------------------------------Shader Programs----------------------------------------//

	// Compile and link shaders here ...
	int shaderProgramBasic = compileAndLinkShaders("Comp371Basic.vshader", "Comp371Basic.fshader");
	int shaderProgramTexture = compileAndLinkShaders("Comp371Texture.vshader", "Comp371Texture.fshader");
	int shaderProgramShadow = compileAndLinkShaders("Comp371Shadow.vshader", "Comp371Shadow.fshader");
	int shaderProgramLightSource = compileAndLinkShaders("Comp371LightSource.vshader", "Comp371LightSource.fshader");
	int shaderProgramParticles = compileAndLinkShaders("Comp371TextureParticles.vshader", "Comp371TextureParticles.fshader");

	//-----------------------------------------VAOs--------------------------------------------//

		//Setup models
#if defined(PLATFORM_OSX)
	string spherePath = "Models/UVSphereTriangle.obj";
	string cubePath = "Models/cube.obj";
	string trunkPath = "Models/trunk.obj";
	string leavesPath = "Models/leaves.obj";
	string wolfPath = "Models/wolf.obj";
#else
	string spherePath = "../Assets/Models/UVSphereTriangle.obj";
	string cubePath = "../Assets/Models/cube.obj";
	string trunkPath = "../Assets/Models/trunk.obj";
	string leavesPath = "../Assets/Models/leaves.obj";
	string wolfPath = "../Assets/Models/wolf.obj";
#endif
	//light Variables
	vec3 lightPos = vec3(0.001f, 200.0f, -150.0f);
	vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
	
	//Initial LightPos and light color
	setVec3(shaderProgramBasic, "lightPos", lightPos);
	setVec3(shaderProgramTexture, "lightPos", lightPos);
	setVec3(shaderProgramTexture, "lightColor", lightColor);
	setVec3(shaderProgramBasic, "lightPos", lightColor);

	int sphereVertices;
	GLuint vaoSphereModel = setupModelEBO(spherePath, sphereVertices);

	int cubeVertices;
	GLuint vaoCubeModel = setupModelEBO(cubePath, cubeVertices);

	int trunkVertices;
	GLuint vaoTrunkModel = setupModelEBO(trunkPath, trunkVertices);

	int leavesVertices;
	GLuint vaoLeavesModel = setupModelEBO(leavesPath, leavesVertices);

	int wolfVertices;
	GLuint vaoWolfModel = setupModelEBO(wolfPath, wolfVertices);

	int vaoCube = createVertexArrayObjectCube();
	int vaoGround = createVertexArrayObjectGround();
	int vaoSnow = createVertexArrayObjectParticles();

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
	vec3 cameraPosition(0.6f, 1.0f, 1.0f);
	vec3 cameraLookAt(0.0f, 0.0f, -1.0f);
	vec3 cameraUp(0.0f, 1.0f, 0.0f);
	vec3 cameraTarget = vec3(0.0f, 0.0f, 0.0f);
	vec3 cameraDirection = normalize(cameraPosition - cameraTarget);

	// THE FOLLOWING IS IMPORTANT FOR THE LIGHTING
	setVec3(shaderProgramBasic, "viewPos", cameraPosition);
	setVec3(shaderProgramTexture, "viewPos", cameraPosition);

	// Other camera parameters
	float cameraSpeed = 2.0f;
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
	setMat4(shaderProgramParticles, "projectionMatrix", projectionMatrix);

	//---------------------------------------ViewMatrix-------------------------------------------//

	// Set initial view matrix
	mat4 viewMatrix = lookAt(cameraPosition,  // eye
		 cameraLookAt,  // center
		cameraUp); // up

	setMat4(shaderProgramBasic, "viewMatrix", viewMatrix);
	setMat4(shaderProgramTexture, "viewMatrix", viewMatrix);
	setMat4(shaderProgramLightSource, "viewMatrix", viewMatrix);
	setMat4(shaderProgramParticles, "viewMatrix", viewMatrix);

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

	// The following two lines makes the snow particles look better
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//---------------------------------------Miscellaneous------------------------------------//

	// Put future variables here

	bool snowEnabled = true; // Used for toggling snow on/off
	bool snowPressed = false;// Goes with above /\

	ISoundEngine* SoundEngine = createIrrKlangDevice();
	SoundEngine->play2D("../Assets/Audio/piano.mp3", GL_TRUE);

	//-----------------------------------------Fog ------------------------------------------//

	int fogEnabled = 0; //int controlling fog switch

	setInt(shaderProgramBasic, "fogEnabled", fogEnabled);
	setInt(shaderProgramTexture, "fogEnabled", fogEnabled);
	setInt(shaderProgramParticles, "fogEnabled", fogEnabled);

	float fogDensity = .014f; //default density then set the float uni

	setFloat(shaderProgramBasic, "density", fogDensity);
	setFloat(shaderProgramTexture, "density", fogDensity);
	setFloat(shaderProgramParticles, "density", fogDensity);

	//--------------------------------------Create trees----------------------------------------//

	vector<vector<Model>> models(4); // Store all trees in this vector
	int numberOfTrees = 75;

	int x, z; // Coordinates for the trees
	float scaleFactor; // Random scale size for each tree
	for (int i = 0; i < numberOfTrees; i++) {
		while (true) {
			bool tooClose = false;
			x = (rand() % 101); // Generate number from 0 to 100
			x -= 50; // Make its range from -50 to 50
			z = (rand() % 101);
			z -= 50;

			for (int j = 0; j < 4; j++) {// 4 because there are 4 quadrants
				for (int k = 0; k < models[j].size(); k++) {
					float distance = sqrt(pow((models[j][k].box.position.x - x), 2) +
						pow((models[j][k].box.position.z - z), 2));
					if (distance <= 3.0) {
						tooClose = true;
						break;
					}
				}
			}
			if (!tooClose) {
				break;
			}
		}

		scaleFactor = (int) ((rand() % 6) + 5); // Generate number from 5 to 10
		scaleFactor = (float) scaleFactor / 10; // Get value from 0.5 to 1.0

		Tree tree(vec3(x, 0.0f, z), vec3(scaleFactor, scaleFactor, scaleFactor));

		// Insert the tree into the vector of its quadrant
		models[getCurrentQuadrant(vec3(x, 0.0f, z))].push_back(tree);
	}

	//--------------------------------------Create wolf-----------------------------------------//

	Wolf wolf(vec3(20.0f, 3.5f, -20.0f), vec3(1.0f, 1.0f, 1.0f));
	models[0].push_back(wolf);

	//--------------------------------------Create snow particles-------------------------------//

	vector<Particle> snowParticles;
	for (int i = 0; i < 1000; i++) {// create 1000 snow particles
		snowParticles.push_back(Particle()); // Generates random positioning in the constructor
	}

	//----------------------------------------------------------------------------------------//

	// Entering Game Loop
	while (!glfwWindowShouldClose(window))
	{
		// Frame time calculation
		float dt = glfwGetTime() - lastFrameTime;
		lastFrameTime += dt;
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//-----------------------------------------SHADOWS--------------------------------------//

		
		mat4 lightProjectionMatrix = ortho(-100.0f, 100.0f, -100.0f, 100.0f, 1.0f, 400.0f);
		mat4 lightViewMatrix = lookAt(lightPos, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
		mat4 lightSpaceMatrix = lightProjectionMatrix * lightViewMatrix;

		
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
		mat4 worldMatrix;
		glBindVertexArray(vaoGround);
		glUseProgram(shaderProgramShadow);

		mat4 groundWorldMatrix = translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f)) *
			scale(mat4(1.0f), vec3(50.0f, 1.0f, 50.0f)); // 100 * 100 grid now
		setMat4(shaderProgramShadow, "worldMatrix", groundWorldMatrix);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		//----------------------------------Draw trees shadow----------------------------------//

		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < models[i].size(); j++) {
				worldMatrix = translate(mat4(1.0f), models[i][j].translationVector) *
					scale(mat4(1.0f), models[i][j].scaleVector);
				setMat4(shaderProgramShadow, "worldMatrix", worldMatrix);

				// Draw trunk
				glBindVertexArray(vaoTrunkModel);
				glDrawElements(GL_TRIANGLES, trunkVertices, GL_UNSIGNED_INT, 0);

				// Draw leaves
				glBindVertexArray(vaoLeavesModel);
				glDrawElements(GL_TRIANGLES, leavesVertices, GL_UNSIGNED_INT, 0);
			}
		}

		//----------------------------------Draw wolf shadow----------------------------------//

		setMat4(shaderProgramShadow, "worldMatrix", translate(mat4(1.0f), wolf.translationVector)*
			scale(mat4(1.0f), wolf.scaleVector));
		glBindVertexArray(vaoWolfModel);
		glDrawElements(GL_TRIANGLES, wolfVertices, GL_UNSIGNED_INT, 0);

		//--------------------------------NOW DO ACTUAL RENDERING-------------------------------//

		// Cull backside again for the actual rendering
		glCullFace(GL_BACK);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

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

		//-----------------------------------Draw Trees---------------------------------------//

		for (int i = 0; i < 4; i++) {
			int size = models[i].size();
			if (i == 0) {
				size--;
			}
			for (int j = 0; j < size; j++) {
				worldMatrix = translate(mat4(1.0f), models[i][j].translationVector) *
					scale(mat4(1.0f), models[i][j].scaleVector);
				setMat4(shaderProgramTexture, "worldMatrix", worldMatrix);

				// Draw trunk
				glActiveTexture(GL_TEXTURE0 + 4);
				glBindTexture(GL_TEXTURE_2D, trunkTextureID);
				setTexture(shaderProgramTexture, "textureSampler", 4);
				glBindVertexArray(vaoTrunkModel);
				glDrawElements(GL_TRIANGLES, trunkVertices, GL_UNSIGNED_INT, 0);

				// Draw leaves
				glActiveTexture(GL_TEXTURE0 + 1);
				glBindTexture(GL_TEXTURE_2D, grassTextureID);
				setTexture(shaderProgramTexture, "textureSampler", 1);
				glBindVertexArray(vaoLeavesModel);
				glDrawElements(GL_TRIANGLES, leavesVertices, GL_UNSIGNED_INT, 0);
			}
		}

		//---------------------------------Draw Snow Particles----------------------------------//
		
		// Alternatively, we could use GL_ONE_MINUS_CONSTANT_COLOR instead of GL_DST_ALPHA
		if (snowEnabled) {
			glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
			glActiveTexture(GL_TEXTURE0 + 2);
			glBindTexture(GL_TEXTURE_2D, snowflakeTextureID);
			glBindVertexArray(vaoSphereModel);
			setTexture(shaderProgramParticles, "textureSampler", 2);
			for (int i = 0; i < snowParticles.size(); i++) {
				snowParticles[i].update(dt, cameraPosition);
				worldMatrix = translate(mat4(1.0f), snowParticles[i].position) *
					scale(mat4(1.0f), vec3(0.08f, 0.08f, 0.08f));
				setMat4(shaderProgramParticles, "worldMatrix", worldMatrix);
				glDrawElements(GL_TRIANGLES, sphereVertices, GL_UNSIGNED_INT, 0);
			}
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}

		//---------------------------------Draw Wolf--------------------------------------------//

		glActiveTexture(GL_TEXTURE0 + 5);
		glBindTexture(GL_TEXTURE_2D, marbleTextureID);
		setTexture(shaderProgramTexture, "textureSampler", 5);
		setMat4(shaderProgramTexture, "worldMatrix", translate(mat4(1.0f), wolf.translationVector) * 
		scale(mat4(1.0f), wolf.scaleVector));
		glBindVertexArray(vaoWolfModel);
		glDrawElements(GL_TRIANGLES, wolfVertices, GL_UNSIGNED_INT, 0);

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
		
		if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) 
		{
			 lightPos = vec3(100.0f, 50.0f, -150.0f);
			 lightColor = vec3(1.0f, 0.5f, 0.0f);
			setVec3(shaderProgramBasic, "lightPos", lightPos);
			setVec3(shaderProgramBasic, "lightColor", lightColor);
			setVec3(shaderProgramTexture, "lightPos",lightPos);
			setVec3(shaderProgramTexture, "lightColor", lightColor);
			
		}
		if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) 
		{
			 lightPos = vec3(0.001f, 200.0f, -150.0f);
			 lightColor = vec3(1.0f, 1.0f, 1.0f);
			 setVec3(shaderProgramBasic, "lightPos", lightPos);
			 setVec3(shaderProgramBasic, "lightColor", lightColor);
			 setVec3(shaderProgramTexture, "lightPos", lightPos);
			 setVec3(shaderProgramTexture, "lightColor", lightColor);
		}
		if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
		{
			lightPos = vec3(-100.0f, 200.0f, -20.0f);
			lightColor = vec3(1.0f, 0.5f, 0.0f);
			setVec3(shaderProgramBasic, "lightPos", lightPos);
			setVec3(shaderProgramBasic, "lightColor", lightColor);
			setVec3(shaderProgramTexture, "lightPos", lightPos);
			setVec3(shaderProgramTexture, "lightColor", lightColor);
		}

		if(glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) // regenerate grass
		{
			grassSeed = rand() % (3000000 - 2000000 + 1) + 2000000;
			grassZoom = rand() % (12 - 1 + 1) + 1;
			if (snowEnabled) {
				float lightColor[3] = { 255. / 255., 255. / 255., 255. / 255. };
				float darkColor[3] = { 147. / 255.,231. / 255., 251 / 255. };
				grassTextureID = makeNoiseTexture(grassSeed, grassZoom, grassPersistence, lightColor, darkColor);
			}
			else {
				float lightColor[3] = { 50. / 255.,156. / 255.,50. / 255. };
				float darkColor[3] = { 34. / 255.,96. / 255.,34. / 255. };
				grassTextureID = makeNoiseTexture(grassSeed, grassZoom, grassPersistence, lightColor, darkColor);
			}
			glActiveTexture(GL_TEXTURE0 + 1);
			glBindTexture(GL_TEXTURE_2D, grassTextureID);
		}

		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) // move camera to the left
		{
			cameraPosition -= cameraSideVector * currentCameraSpeed * dt;

			int currentQuadrant = getCurrentQuadrant(cameraPosition);
			for (int i = 0; i < models[currentQuadrant].size(); i++) {
				if (checkCollision(cameraPosition, models[currentQuadrant][i].box)) {
					cameraPosition += cameraSideVector * currentCameraSpeed * dt;
					break;
				}
			}
		}

		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) // move camera to the right
		{
			cameraPosition += cameraSideVector * currentCameraSpeed * dt;

			int currentQuadrant = getCurrentQuadrant(cameraPosition);
			for (int i = 0; i < models[currentQuadrant].size(); i++) {
				if (checkCollision(cameraPosition, models[currentQuadrant][i].box)) {
					cameraPosition -= cameraSideVector * currentCameraSpeed * dt;
					break;
				}
			}
		}

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) // move camera forward
		{
			cameraPosition += cameraLookAt * currentCameraSpeed * dt;
			cameraPosition.y = std::max(0.1f, cameraPosition.y); // Make sure it doesn't go below ground

			int currentQuadrant = getCurrentQuadrant(cameraPosition);
			for (int i = 0; i < models[currentQuadrant].size(); i++) {
				if (checkCollision(cameraPosition, models[currentQuadrant][i].box)) {
					cameraPosition -= cameraLookAt * currentCameraSpeed * dt;
					break;
				}
			}
		}

		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) // move camera backward
		{
			cameraPosition -= cameraLookAt * currentCameraSpeed * dt;
			cameraPosition.y = std::max(0.1f, cameraPosition.y); // Make sure it doesn't go below ground

			int currentQuadrant = getCurrentQuadrant(cameraPosition);
			for (int i = 0; i < models[currentQuadrant].size(); i++) {
				if (checkCollision(cameraPosition, models[currentQuadrant][i].box)) {
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
				setInt(shaderProgramParticles, "fogEnabled", fogEnabled);
			}
			else
			{
				fogEnabled = 0;
				setInt(shaderProgramBasic, "fogEnabled", fogEnabled);
				setInt(shaderProgramTexture, "fogEnabled", fogEnabled);
				setInt(shaderProgramParticles, "fogEnabled", fogEnabled);
			}
		}

		if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) //increase fog density incrementally 
		{
			if (fogDensity <= 1)
			{
				fogDensity += .001f;
				setFloat(shaderProgramBasic, "density", fogDensity);
				setFloat(shaderProgramTexture, "density", fogDensity);
				setFloat(shaderProgramParticles, "density", fogDensity);
			}
			}

		if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) //decrease fog density incrementally 
		{
			if (fogDensity >= 0)
			{
				fogDensity -= .001f;
				setFloat(shaderProgramBasic, "density", fogDensity);
				setFloat(shaderProgramTexture, "density", fogDensity);
				setFloat(shaderProgramParticles, "density", fogDensity);
			}
			if (fogDensity < 0) {
				fogDensity = 0;

				setFloat(shaderProgramBasic, "density", fogDensity);
				setFloat(shaderProgramTexture, "density", fogDensity);
				setFloat(shaderProgramParticles, "density", fogDensity);

			 }


		}




		// Toggle snow on/off
		if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
			if (!snowPressed) {
				snowPressed = true;
				snowEnabled = !snowEnabled;
			}
			if (snowEnabled) {
				float lightColor[3] = { 255. / 255., 255. / 255., 255. / 255. };
				float darkColor[3] = { 147. / 255.,231. / 255., 251 / 255. };
				grassTextureID = makeNoiseTexture(grassSeed, grassZoom, grassPersistence, lightColor, darkColor);
			}
			else {
				float lightColor[3] = { 50. / 255.,156. / 255.,50. / 255. };
				float darkColor[3] = { 34. / 255.,96. / 255.,34. / 255. };
				grassTextureID = makeNoiseTexture(grassSeed, grassZoom, grassPersistence, lightColor, darkColor);
			}
			glActiveTexture(GL_TEXTURE0 + 1);
			glBindTexture(GL_TEXTURE_2D, grassTextureID);
		}
		else {
			snowPressed = false;
		}



		setVec3(shaderProgramBasic, "viewPos", cameraPosition);
		setVec3(shaderProgramTexture, "viewPos", cameraPosition);

		setVec3(shaderProgramBasic, "lightPos", lightPos);
		setVec3(shaderProgramBasic, "lightPos", lightColor);
		setVec3(shaderProgramTexture, "lightPos", lightPos);
		setVec3(shaderProgramTexture, "lightColor", lightColor);

		mat4 viewMatrix = lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);

		setMat4(shaderProgramBasic, "viewMatrix", viewMatrix);
		setMat4(shaderProgramTexture, "viewMatrix", viewMatrix);
		setMat4(shaderProgramLightSource, "viewMatrix", viewMatrix);
		setMat4(shaderProgramParticles, "viewMatrix", viewMatrix);
		glUseProgram(shaderProgramBasic);

	}


	// Shutdown GLFW
	glfwTerminate();

	return 0;
}

