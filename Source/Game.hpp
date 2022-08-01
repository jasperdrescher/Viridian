#pragma once

#include "MapLayer.hpp"

#include <glm/matrix.hpp>

#include <memory>
#include <string>
#include <vector>

struct GLFWwindow;

class Game final
{
public:
	Game(GLFWwindow* aWindow);
	~Game();

	void Run();

private:
	void HandleEvents() {}
	void Update(float aDeltaTime) {}
	void Draw() const;
	void LoadMap();
	void InitializeGL(const tmx::Map&);
	void LoadShader();
	void LoadTexture(const std::string&);

	std::vector<std::unique_ptr<MapLayer>> myMapLayers;
	std::vector<unsigned> myTileTextureIdentifiers;
	glm::mat4 myProjectionMatrix;
	glm::mat4 myViewMatrix;
	glm::mat4 myModelMatrix;
	GLFWwindow* myGLFWWindow;
	unsigned myShaderProgramIdentifier;
};
