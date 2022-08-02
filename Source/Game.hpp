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
	Game();
	~Game();

	void Initialize();
	void Run();

private:
	static void HandleEvents() {}
	static void Update(float aDeltaTime) {}
	void Draw() const;
	void LoadMap();
	void InitializeGL(const tmx::Map& aMap);
	void LoadShader();
	void LoadTexture(const std::string& aFilepath);
	void PrintDebugInfo();

	std::vector<std::unique_ptr<MapLayer>> myMapLayers;
	std::vector<unsigned int> myTileTextureIdentifiers;
	glm::mat4 myProjectionMatrix;
	glm::mat4 myViewMatrix;
	glm::mat4 myModelMatrix;
	GLFWwindow* myGLFWWindow;
	unsigned int myShaderProgramIdentifier;
};
