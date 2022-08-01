#pragma once

#include <glm/matrix.hpp>

#include "MapLayer.hpp"

#include <memory>
#include <string>
#include <vector>

struct GLFWwindow;

class Game final
{
public:
	Game(GLFWwindow* aWindow);
	~Game();
	
	void run();
	
private:

	void doEvents();
	void update(float);
	void draw();
	
	std::vector<std::unique_ptr<MapLayer>> m_mapLayers;
	void loadMap();
	
	glm::mat4 m_projectionMatrix;
	
	unsigned myShaderProgramIdentifier;
	std::vector<unsigned> m_tileTextures;
	void initGLStuff(const tmx::Map&);
	void loadShader();
	void loadTexture(const std::string&);

private:
	GLFWwindow* myGLFWWindow;
};
