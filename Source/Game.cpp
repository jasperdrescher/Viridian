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

namespace
{
	const float timePerFrame = 1.f / 60.f;
}

Game::Game(GLFWwindow* aWindow)
	: myShaderProgramIdentifier(0)
  , myGLFWWindow(aWindow) { }

Game::~Game()
{
	if (myShaderProgramIdentifier)
	{
		glDeleteProgram(myShaderProgramIdentifier);
	}
	for (const auto& t : m_tileTextures)
	{
		glDeleteTextures(1, &t);
	}
}

//public
void Game::run()
{
	printf("Current working directory: %s\n", std::filesystem::current_path().string().c_str());

	loadMap();

	auto previousTime = std::chrono::high_resolution_clock::now();
	while (!glfwWindowShouldClose(myGLFWWindow))
	{
		auto currentTime = std::chrono::high_resolution_clock::now();
		auto elapsedTime = currentTime - previousTime;
		const float deltaTime = std::chrono::duration<float>(elapsedTime).count();
		previousTime = currentTime;

		doEvents();
		update(deltaTime);
		draw();
	}
}

//private
void Game::doEvents() { }

void Game::update(float dt) {}

void Game::draw()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(myShaderProgramIdentifier);

	for (const auto& layer : m_mapLayers)
	{
		layer->draw();
	}

	glfwSwapBuffers(myGLFWWindow);
	glfwPollEvents();
}


void Game::loadMap()
{
	const std::string filePath = "Data/demo.tmx";
	if (!FileUtility::Exists(filePath.c_str()))
		return;

	tmx::Map map;
	map.load(filePath);

	//create shared resources, shader and tileset textures
	initGLStuff(map);

	//create a drawable object for each tile layer
	const auto& layers = map.getLayers();
	for (auto i = 0u; i < layers.size(); ++i)
	{
		if (layers[i]->getType() == tmx::Layer::Type::Tile)
		{
			m_mapLayers.emplace_back(std::make_unique<MapLayer>(map, i, m_tileTextures));
		}
	}
}


void Game::initGLStuff(const tmx::Map& map)
{
	m_projectionMatrix = glm::ortho(0.f, 800.f, 600.f, 0.f, -0.1f, 100.f);

	loadShader();
	glUseProgram(myShaderProgramIdentifier);
	glUniformMatrix4fv(glGetUniformLocation(myShaderProgramIdentifier, "u_projectionMatrix"), 1, GL_FALSE, &m_projectionMatrix[0][0]);

	// We'll make sure the current tile texture is active in 0,
	// and lookup texture is active in 1 in MapLayer::draw()
	glUniform1i(glGetUniformLocation(myShaderProgramIdentifier, "u_tileMap"), 0);
	glUniform1i(glGetUniformLocation(myShaderProgramIdentifier, "u_lookupMap"), 1);

	const auto& tilesets = map.getTilesets();
	for (const auto& ts : tilesets)
	{
		loadTexture(ts.getImagePath());
	}

	glClearColor(0.6f, 0.8f, 0.92f, 1.f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);
}

void Game::loadShader()
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

void Game::loadTexture(const std::string& path)
{
	m_tileTextures.emplace_back(0);
	unsigned& textureIdentifier = m_tileTextures.back();

	int width = 0;
	int height = 0;
	int numberOfChannels = 0;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &numberOfChannels, 4);
	if (!data)
	{
		printf("Failed to load %s\n", path.c_str());
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
