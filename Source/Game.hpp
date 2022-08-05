#pragma once

#include "MapLayer.hpp"

#include <glm/matrix.hpp>

#include <memory>
#include <string>
#include <vector>

struct GLFWwindow;
class Camera;

class Game final
{
public:
	Game();
	~Game();

	void Initialize();
	void Run();

private:
	void Update(const float aDeltaTime);
	void Draw() const;
	void LoadMap();
	void InitializeGL(const tmx::Map& aMap);
	void LoadShader();
	void LoadTexture(const std::string& aFilepath);
	static void KeyCallback(GLFWwindow* aWindow, int aKey, int aScancode, int anAction, int aMode);
	static void PrintDebugInfo();

	std::vector<std::unique_ptr<MapLayer>> myMapLayers;
	std::vector<unsigned int> myTileTextureIdentifiers;
	glm::mat4 myModelMatrix;
	glm::mat4 myModelViewProjectionMatrix;
	glm::vec2 myWindowSize;
	GLFWwindow* myGLFWWindow;
	Camera* myCamera;
	unsigned int myShaderProgramIdentifier;
};
