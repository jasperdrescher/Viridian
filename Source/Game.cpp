#include "Game.hpp"
#include "FileUtility.hpp"
#include "Shader.hpp"
#include "GLDebugUtility.hpp"

#include <GLFW/glfw3.h>
#include <chrono>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <tmxlite/Map.hpp>

Game::Game()
	: myProjectionMatrix(0.0f)
	, myViewMatrix(0.0f)
	, myModelMatrix(0.0f)
	, myGLFWWindow(nullptr)
	, myShaderProgramIdentifier(0)
{}

Game::~Game()
{
	if (myShaderProgramIdentifier)
		glDeleteProgram(myShaderProgramIdentifier);

	for (const unsigned int& textureIdentifier : myTileTextureIdentifiers)
		glDeleteTextures(1, &textureIdentifier);

	glfwTerminate();
}

void Game::Initialize()
{
	glfwSetErrorCallback(GLDebugUtility::ErrorCallback);

	if (!glfwInit())
	{
		glfwTerminate();
		return;
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	myGLFWWindow = glfwCreateWindow(800, 600, "Viridian Debug x64", nullptr, nullptr);
	if (!myGLFWWindow)
	{
		glfwTerminate();
		printf("Failed to create a window\n");
		return;
	}

	glfwMakeContextCurrent(myGLFWWindow);

	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
	{
		glfwTerminate();
		printf("Failed to load GL\n");
		return;
	}

	PrintDebugInfo();

	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_FALSE);
	glDebugMessageCallback(GLDebugUtility::ErrorCallback, nullptr);
}

void Game::Run()
{
	printf("Current working directory: %s\n", std::filesystem::current_path().string().c_str());

	LoadMap();

	std::chrono::time_point<std::chrono::steady_clock> previousTime = std::chrono::high_resolution_clock::now();
	while (!glfwWindowShouldClose(myGLFWWindow))
	{
		std::chrono::time_point<std::chrono::steady_clock> currentTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<long long, std::ratio<1, 1000000000>> elapsedTime = currentTime - previousTime;
		const float deltaTime = std::chrono::duration<float>(elapsedTime).count();
		previousTime = currentTime;

		HandleEvents();
		Update(deltaTime);
		Draw();
	}
}

void Game::Draw() const
{
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(myShaderProgramIdentifier);

	for (const std::unique_ptr<MapLayer>& layer : myMapLayers)
		layer->Draw();

	glfwSwapBuffers(myGLFWWindow);
	glfwPollEvents();
}

void Game::LoadMap()
{
	const std::string filePath = "Data/platform.tmx";
	if (!FileUtility::Exists(filePath.c_str()))
		return;

	tmx::Map map;
	map.load(filePath);

	InitializeGL(map);

	const std::vector<tmx::Layer::Ptr>& layers = map.getLayers();
	for (unsigned int i = 0; i < layers.size(); ++i)
	{
		if (layers[i]->getType() == tmx::Layer::Type::Tile)
			myMapLayers.emplace_back(std::make_unique<MapLayer>(map, i, myTileTextureIdentifiers));
	}
}

void Game::InitializeGL(const tmx::Map& aMap)
{
	myProjectionMatrix = glm::ortho(0.0f, 800.0f, 600.0f, 0.0f, -0.1f, 100.0f);
	constexpr glm::vec3 cameraPosition = glm::vec3(200.0f, 2000.0f, 0.0f);
	constexpr glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	myViewMatrix = glm::lookAt(cameraPosition, cameraPosition + cameraFront, glm::vec3(0.0f, 1.0f, 0.0f));
	myModelMatrix = glm::mat4(1.0f);

	LoadShader();
	glUseProgram(myShaderProgramIdentifier);
	glm::mat4x4 mvp = myProjectionMatrix * myViewMatrix * myModelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(myShaderProgramIdentifier, "u_MVP"), 1, GL_FALSE, glm::value_ptr(mvp));

	// We'll make sure the current tile texture is active in 0,
	// and lookup texture is active in 1 in MapLayer::draw()
	glUniform1i(glGetUniformLocation(myShaderProgramIdentifier, "u_tileMap"), 0);
	glUniform1i(glGetUniformLocation(myShaderProgramIdentifier, "u_lookupMap"), 1);

	const std::vector<tmx::Tileset>& tilesets = aMap.getTilesets();
	for (const tmx::Tileset& tileset : tilesets)
		LoadTexture(tileset.getImagePath());

	glClearColor(0.6f, 0.8f, 0.92f, 1.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);
}

void Game::LoadShader()
{
	myShaderProgramIdentifier = glCreateProgram();
	Shader vertexShader;
	Shader fragmentShader;
	const std::string vertexShaderData = FileUtility::ReadFile("Data/VertexShader.glsl");
	const std::string fragmentShaderData = FileUtility::ReadFile("Data/FragmentShader.glsl");
	vertexShader.AttachShader(myShaderProgramIdentifier, GL_VERTEX_SHADER, vertexShaderData.c_str());
	fragmentShader.AttachShader(myShaderProgramIdentifier, GL_FRAGMENT_SHADER, fragmentShaderData.c_str());

	glLinkProgram(myShaderProgramIdentifier);

	vertexShader.CheckShaderLinkStatus(myShaderProgramIdentifier);
	fragmentShader.CheckShaderLinkStatus(myShaderProgramIdentifier);

	glBindAttribLocation(myShaderProgramIdentifier, 0, "a_position");
	glBindAttribLocation(myShaderProgramIdentifier, 1, "a_texCoord");
}

void Game::LoadTexture(const std::string& aFilepath)
{
	myTileTextureIdentifiers.emplace_back(0);
	unsigned int& textureIdentifier = myTileTextureIdentifiers.back();

	int width = 0;
	int height = 0;
	int numberOfChannels = 0;
	unsigned char* data = stbi_load(aFilepath.c_str(), &width, &height, &numberOfChannels, 4);
	if (!data)
	{
		printf("Failed to load %s\n", aFilepath.c_str());
		return;
	}

	glGenTextures(1, &textureIdentifier);
	glBindTexture(GL_TEXTURE_2D, textureIdentifier);

	const GLint internalformat = numberOfChannels == 3 ? GL_RGB : GL_RGBA;
	const GLenum format = numberOfChannels == 3 ? GL_RGB : GL_RGBA;
	glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(data);
}

void Game::PrintDebugInfo()
{
	printf("OpenGL %s\n", glGetString(GL_VERSION));
	printf("GLSL %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	printf("Vendor %s\n", glGetString(GL_VENDOR));
	printf("Renderer %s\n", glGetString(GL_RENDERER));
	int major, minor, revision;
	glfwGetVersion(&major, &minor, &revision);
	printf("GLFW %d.%d.%d\n", major, minor, revision);
}
