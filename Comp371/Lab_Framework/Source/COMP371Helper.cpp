#include "COMP371Helper.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

GLFWwindow* window = NULL;

struct VertexTexture {
	VertexTexture(vec3 _position, vec3 _color, vec3 _normal, vec2 _uv)
		: position(_position), color(_color), normal(_normal), uv(_uv) {}

	glm::vec3 position;
	glm::vec3 color;
	glm::vec3 normal;
	glm::vec2 uv;
};

bool initContext() {     // Initialize GLFW and OpenGL version
	glfwInit();

#if defined(PLATFORM_OSX)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
	// On windows, we set OpenGL version to 2.1, to support more hardware
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#endif

	// Create Window and rendering context using GLFW, resolution is 800x600
	window = glfwCreateWindow(800, 600, "Comp371 - Lab 03", NULL, NULL);
	if (window == NULL)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);

	// @TODO 3 - Disable mouse cursor
	// ...

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		std::cerr << "Failed to create GLEW" << std::endl;
		glfwTerminate();
		return false;
	}
	return true;
}

int compileAndLinkShaders(string vertexPath, string fragmentPath)
{
	// compile and link shader program
	// return shader program id
	// ------------------------------------

	// vertex shader
	int vertexShader = glCreateShader(GL_VERTEX_SHADER);

#if defined(PLATFORM_OSX)
	std::string shaderPathPrefix = "Shaders/";
#else
	std::string shaderPathPrefix = "../Assets/Shaders/";
#endif

	string vertexShaderPath = shaderPathPrefix + vertexPath;

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertexShaderPath, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::string Line = "";
		while (getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertexShaderPath.c_str());
		getchar();
		exit(-1);
	}

	const char* vertexShaderSource = VertexShaderCode.c_str();
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	// check for shader compile errors
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// fragment shader
	int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	string fragmentShaderPath = shaderPathPrefix + fragmentPath;

	std::string fragmentShaderCode;
	std::ifstream fragmentShaderStream(fragmentShaderPath, std::ios::in);
	if (fragmentShaderStream.is_open()) {
		std::string Line = "";
		while (getline(fragmentShaderStream, Line))
			fragmentShaderCode += "\n" + Line;
		fragmentShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", fragmentShaderPath.c_str());
		getchar();
		exit(-1);
	}

	const char* fragmentShaderSource = fragmentShaderCode.c_str();
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	// check for shader compile errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// link shaders
	int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	// check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}

int createVertexArrayObjectGround() {

	VertexTexture vertexArray[] = {
		// position,                        color,                   normal                 uv
		VertexTexture(vec3(-1.0f, 0.0f, 1.0f), vec3(1.0f, 1.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 0.0f)), // Bottom left point
		VertexTexture(vec3(1.0f, 0.0f, 1.0f), vec3(1.0f, 1.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f), vec2(1.0f, 0.0f)), // Bottom right point
		VertexTexture(vec3(1.0f, 0.0f, -1.0f), vec3(1.0f, 1.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f), vec2(1.0f, 1.0f)), // Top right point

		VertexTexture(vec3(-1.0f, 0.0f, 1.0f), vec3(1.0f, 1.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 0.0f)), // Bottom left point
		VertexTexture(vec3(1.0f, 0.0f, -1.0f), vec3(1.0f, 1.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f), vec2(1.0f, 1.0f)), // Top right point
		VertexTexture(vec3(-1.0f, 0.0f, -1.0f), vec3(1.0f, 1.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 1.0f)), // Top Left point
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

	// 1st attribute buffer : vertex Positions
	glVertexAttribPointer(0,              // attribute. No particular reason for 0, but must match the layout in the shader.
		3,              // size
		GL_FLOAT,       // type
		GL_FALSE,       // normalized?
		sizeof(VertexTexture), // stride
		(void*)0        // array buffer offset
	);
	glEnableVertexAttribArray(0);

	// 2nd attribute buffer : vertex color
	glVertexAttribPointer(1,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(VertexTexture),
		(void*)sizeof(vec3)    // Color is Offseted by vec3 (see class Vertex)
	);
	glEnableVertexAttribArray(1);


	// 3rd attribute buffer : vertex normal
	glVertexAttribPointer(2,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(VertexTexture),
		(void*)(2 * sizeof(vec3)) // Normal is Offseted by 2 vec3 (see class Vertex)
	);
	glEnableVertexAttribArray(2);

	// 4th attribute buffer : uv
	glVertexAttribPointer(3,
		2, // Size is 2 because it's a vec2
		GL_FLOAT,
		GL_FALSE,
		sizeof(VertexTexture),
		(void*)(3 * sizeof(vec3)) // Normal is Offseted by 3 vec3 (see class VertexTexture)
	);
	glEnableVertexAttribArray(3);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glBindVertexArray(0);

	return vertexArrayObject;
}

GLuint loadTexture(const char* filename)
{
	// Step1 Create and bind textures
	GLuint textureId = 0;
	glGenTextures(1, &textureId);
	assert(textureId != 0);


	glBindTexture(GL_TEXTURE_2D, textureId);

	// Step2 Set filter parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	// Step3 Load Textures with dimension data
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
	if (!data)
	{
		std::cerr << "Error::Texture could not load texture file:" << filename << std::endl;
		return 0;
	}

	// Step4 Upload the texture to the PU
	GLenum format = 0;
	if (nrChannels == 1)
		format = GL_RED;
	else if (nrChannels == 3)
		format = GL_RGB;
	else if (nrChannels == 4)
		format = GL_RGBA;
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height,
		0, format, GL_UNSIGNED_BYTE, data);

	glGenerateMipmap(GL_TEXTURE_2D);

	// Step5 Free resources
	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);
	return textureId;
}