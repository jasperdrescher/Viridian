#include "Game.hpp"
#include "FileUtility.hpp"
#include "Shader.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glm/gtc/matrix_transform.hpp>
#include <tmxlite/Map.hpp>

#include <chrono>

Game::Game(GLFWwindow* aWindow)
	: myProjectionMatrix(0.0f)
  , myViewMatrix(0.0f)
  , myModelMatrix(0.0f)
  , myGLFWWindow(aWindow)
  , myShaderProgramIdentifier(0)
{}

Game::~Game()
{
	if (myShaderProgramIdentifier)
		glDeleteProgram(myShaderProgramIdentifier);

	for (const auto& textureIdentifier : myTileTextureIdentifiers)
		glDeleteTextures(1, &textureIdentifier);
}

void Game::Run()
{
	printf("Current working directory: %s\n", std::filesystem::current_path().string().c_str());

	LoadMap();

	auto previousTime = std::chrono::high_resolution_clock::now();
	while (!glfwWindowShouldClose(myGLFWWindow))
	{
		auto currentTime = std::chrono::high_resolution_clock::now();
		auto elapsedTime = currentTime - previousTime;
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

	for (const auto& layer : myMapLayers)
	{
		layer->Draw();
	}

	glfwSwapBuffers(myGLFWWindow);
	glfwPollEvents();
}

void Game::LoadMap()
{
	const std::string filePath = "Data/demo.tmx";
	if (!FileUtility::Exists(filePath.c_str()))
		return;

	tmx::Map map;
	map.load(filePath);

	// Create shared resources, shader and tileset textures
	InitializeGL(map);

	// Create a drawable object for each tile layer
	const auto& layers = map.getLayers();
	for (auto i = 0u; i < layers.size(); ++i)
	{
		if (layers[i]->getType() == tmx::Layer::Type::Tile)
		{
			myMapLayers.emplace_back(std::make_unique<MapLayer>(map, i, myTileTextureIdentifiers));
		}
	}
}

void Game::InitializeGL(const tmx::Map& aMap)
{
	myProjectionMatrix = glm::ortho(0.0f, 800.0f, 600.0f, 0.0f, -0.1f, 100.0f);
	myViewMatrix = glm::lookAt(
		glm::vec3(0.0f, 0.0f, 0.0f), // Camera is at this in World Space
		glm::vec3(0.0f, 0.0f, -1.0f), // and looks at the origin
		glm::vec3(0.0f, 1.0f, 0.0f) // Head is up (set to 0,-1,0 to look upside-down)
	);
	myModelMatrix = glm::mat4(1.0f);
	glm::translate(myViewMatrix, glm::vec3(0.0f, -1.0f, -3.0f));

	LoadShader();
	glUseProgram(myShaderProgramIdentifier);
	glm::mat4x4 mvp = myProjectionMatrix * myViewMatrix * myModelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(myShaderProgramIdentifier, "u_MVP"), 1, GL_FALSE, &mvp[0][0]);

	// We'll make sure the current tile texture is active in 0,
	// and lookup texture is active in 1 in MapLayer::draw()
	glUniform1i(glGetUniformLocation(myShaderProgramIdentifier, "u_tileMap"), 0);
	glUniform1i(glGetUniformLocation(myShaderProgramIdentifier, "u_lookupMap"), 1);

	const auto& tilesets = aMap.getTilesets();
	for (const auto& ts : tilesets)
	{
		LoadTexture(ts.getImagePath());
	}

	glClearColor(0.6f, 0.8f, 0.92f, 1.f);
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
	unsigned& textureIdentifier = myTileTextureIdentifiers.back();

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
