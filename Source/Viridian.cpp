#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <tileson.hpp>

#include <filesystem>
#include <fstream>
#include <string>
#include <sstream>

void ParseTilemap()
{
	const char* settingsSource = "Data/Settings.txt";
	std::ifstream settingsFilestream(settingsSource, std::ifstream::in);
	if (!settingsFilestream.is_open())
	{
		printf("Failed to open %s\n", settingsSource);
		return;
	}

	std::stringstream settingsSourceStringStream;
	settingsSourceStringStream << settingsFilestream.rdbuf();

	settingsFilestream.close();

	const std::string source = settingsSourceStringStream.str();
	if (!std::filesystem::exists(source))
	{
		printf("Failed to open %s\n", source.c_str());
		return;
	}

	std::ifstream filestream(source, std::ifstream::in);
	if (!filestream.is_open())
	{
		printf("Failed to open %s\n", source.c_str());
		return;
	}

	std::stringstream sourceStringStream;
	sourceStringStream << filestream.rdbuf();

	filestream.close();

	const std::string dataString = sourceStringStream.str();
	const unsigned int bufferSize = static_cast<unsigned int>(dataString.length()) + 1;
	char* buffer = new char[bufferSize];
	strcpy_s(buffer, bufferSize, dataString.c_str());

	tson::Tileson tileson;
	std::unique_ptr<tson::Map> map = tileson.parse(buffer, bufferSize);

	if (map->getStatus() != tson::ParseStatus::OK)
	{
		printf("Failed to parse %s\n", source.c_str());
		return;
	}

	for (tson::Layer& layer : map->getLayers())
	{
		if (layer.getType() == tson::LayerType::TileLayer)
		{
			for (auto& [pos, tileObject] : layer.getTileObjects())
			{
				tson::Tileset* localTileset = tileObject.getTile()->getTileset();
				tson::Rect drawingRect = tileObject.getDrawingRect();
				tson::Vector2f position = tileObject.getPosition();
				printf("Tileset: %s\n", localTileset->getImagePath().generic_string().c_str());
				printf("Rect: %i %i %i %i\n", drawingRect.x, drawingRect.y, drawingRect.width, drawingRect.height);
				printf("Position: %0.1f %0.1f\n", position.x, position.y);
			}
		}
	}
}

int main(int /*argc*/, char** /*argv*/)
{
	if (!glfwInit())
	{
		glfwTerminate();
		return -1;
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(640, 480, "Viridian Debug x64", nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
	{
		glfwTerminate();
		return -1;
	}

	ParseTilemap();

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}
