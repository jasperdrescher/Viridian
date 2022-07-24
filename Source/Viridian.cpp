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

void PrintDebugInfo()
{
	printf("OpenGL %s\n", glGetString(GL_VERSION));
	printf("GLSL %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	int major, minor, revision;
	glfwGetVersion(&major, &minor, &revision);
	printf("GLFW %d.%d.%d\n", major, minor, revision);
}

static void GLAPIENTRY ErrorCallback(GLenum aSource, GLenum aType, GLuint anID, GLenum aSeverity, GLsizei /*aLength*/, const GLchar* aMessage, const void* /*aUserParam*/)
{
	std::string source;
	std::string type;
	std::string severity;

	switch (aSource)
	{
		case GL_DEBUG_SOURCE_API:
			source = "API";
			break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
			source = "WINDOW SYSTEM";
			break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER:
			source = "SHADER COMPILER";
			break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:
			source = "THIRD PARTY";
			break;
		case GL_DEBUG_SOURCE_APPLICATION:
			source = "APPLICATION";
			break;
		case GL_DEBUG_SOURCE_OTHER:
			source = "OTHER";
			break;
		default:
			source = "UNKNOWN";
			break;
	}

	switch (aType)
	{
		case GL_DEBUG_TYPE_ERROR:
			type = "ERROR";
			break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			type = "DEPRECATED BEHAVIOR";
			break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			type = "UDEFINED BEHAVIOR";
			break;
		case GL_DEBUG_TYPE_PORTABILITY:
			type = "PORTABILITY";
			break;
		case GL_DEBUG_TYPE_PERFORMANCE:
			type = "PERFORMANCE";
			break;
		case GL_DEBUG_TYPE_OTHER:
			type = "OTHER";
			break;
		case GL_DEBUG_TYPE_MARKER:
			type = "MARKER";
			break;
		default:
			type = "UNKNOWN";
			break;
	}

	switch (aSeverity)
	{
		case GL_DEBUG_SEVERITY_HIGH:
			severity = "HIGH";
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			severity = "MEDIUM";
			break;
		case GL_DEBUG_SEVERITY_LOW:
			severity = "LOW";
			break;
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			severity = "NOTIFICATION";
			break;
		default:
			severity = "UNKNOWN";
			break;
	}

	printf("Message from OpenGL: Severity: %s Type: %s Source: %s Id: 0x%x\n", severity.c_str(), type.c_str(), source.c_str(), anID);
	printf("%s\n", aMessage);
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
	glfwWindowHint(GLFW_SAMPLES, 4);

	GLFWwindow* window = glfwCreateWindow(640, 480, "Viridian Debug x64", nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		printf("Failed to create a window\n");
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
	{
		glfwTerminate();
		printf("Failed to load GL\n");
		return -1;
	}

	PrintDebugInfo();

	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_FALSE);
	glDebugMessageCallback(ErrorCallback, nullptr);

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
