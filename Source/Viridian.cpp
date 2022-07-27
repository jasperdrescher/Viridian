#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <tileson.hpp>

#include <filesystem>
#include <fstream>
#include <string>
#include <sstream>

std::string ReadFile(const char* aFilepath)
{
	if (!std::filesystem::exists(aFilepath))
	{
		printf("Failed to open %s\n", aFilepath);
		return "";
	}

	std::ifstream filestream(aFilepath, std::ifstream::in);
	if (!filestream.is_open())
	{
		printf("Failed to open %s\n", aFilepath);
		return "";
	}

	std::stringstream sourceStringStream;
	sourceStringStream << filestream.rdbuf();

	filestream.close();

	return sourceStringStream.str();
}

struct Tile
{
	tson::Rect myDrawingRectangle;
	tson::Vector2f myPosition;
};

struct Image
{
	std::string myFilepath;
	tson::Vector2i mySize;
};

struct Tilemap
{
	std::vector<Tile> myTiles;
	Image myImage;
};

Tilemap ParseTilemap()
{
	const char* settingsSource = "Data/Settings.txt";
	const std::string tilemapFilepath = ReadFile(settingsSource);
	const std::string dataString = ReadFile(tilemapFilepath.c_str());
	const unsigned int bufferSize = static_cast<unsigned int>(dataString.length()) + 1;
	char* buffer = new char[bufferSize];
	strcpy_s(buffer, bufferSize, dataString.c_str());

	tson::Tileson tileson;
	const std::unique_ptr<tson::Map> map = tileson.parse(buffer, bufferSize);

	Tilemap tilemap;
	if (map->getStatus() != tson::ParseStatus::OK)
	{
		printf("Failed to parse %s\n", tilemapFilepath.c_str());
		return tilemap;
	}

	for (tson::Layer& layer : map->getLayers())
	{
		if (layer.getType() == tson::LayerType::TileLayer)
		{
			for (auto& [pos, tileObject] : layer.getTileObjects())
			{
				const tson::Tileset* localTileset = tileObject.getTile()->getTileset();
				const tson::Rect drawingRect = tileObject.getDrawingRect();
				const tson::Vector2f position = tileObject.getPosition();
				const tson::Vector2i imageSize = localTileset->getImageSize();
				std::string imageFilepath = localTileset->getImagePath().generic_string();

				std::string finalImageFilepath;
				if (!imageFilepath.empty())
				{
					imageFilepath.erase(0, imageFilepath.find_last_of('/') + 1);
					finalImageFilepath = "Data/" + imageFilepath;
					printf("Tileset: %s\n", finalImageFilepath.c_str());
				}
				printf("Rect: %i %i %i %i\n", drawingRect.x, drawingRect.y, drawingRect.width, drawingRect.height);
				printf("Position: %0.1f %0.1f\n", position.x, position.y);

				Tile tile;
				tile.myDrawingRectangle = drawingRect;
				tile.myPosition = position;
				tilemap.myTiles.emplace_back(tile);

				if (tilemap.myImage.myFilepath.empty())
				{
					tilemap.myImage.myFilepath = finalImageFilepath;
					tilemap.myImage.mySize = imageSize;
				}
			}
		}
	}
	return tilemap;
}

int CreateTexture(const std::string& aFilepath)
{
	unsigned int textureIdentifier;
	glGenTextures(1, &textureIdentifier);
	glBindTexture(GL_TEXTURE_2D, textureIdentifier);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	stbi_set_flip_vertically_on_load(1);

	int width = 0;
	int height = 0;
	unsigned char* data = stbi_load(aFilepath.c_str(), &width, &height, nullptr, 3);
	if (!data)
	{
		printf("Failed to load %s\n", aFilepath.c_str());
		return -1;
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(data);

	return static_cast<int>(textureIdentifier);
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

void ErrorCallback(int anError, const char* aDescription)
{
	printf("%i %s", anError, aDescription);
}

void CheckShaderLinkStatus(GLuint aProgramIdentifier)
{
	GLint isLinked = GL_FALSE;
	glGetProgramiv(aProgramIdentifier, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE)
	{
		GLchar infoLog[512];
		glGetProgramInfoLog(aProgramIdentifier, 512, nullptr, infoLog);
		printf("%s\n", infoLog);
	}
}

void CheckShaderCompileStatus(GLuint aShaderIdentifier)
{
	GLint isCompiled = GL_FALSE;
	glGetShaderiv(aShaderIdentifier, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLchar shaderInfoLog[512];
		glGetShaderInfoLog(aShaderIdentifier, 512, nullptr, shaderInfoLog);
		printf("%s\n", shaderInfoLog);
	}
}

void AttachShader(GLuint aProgramIdentifier, GLenum aType, const char* aSource)
{
	const GLuint shaderIdentifier = glCreateShader(aType);
	glShaderSource(shaderIdentifier, 1, &aSource, nullptr);
	glCompileShader(shaderIdentifier);
	CheckShaderCompileStatus(shaderIdentifier);
	glAttachShader(aProgramIdentifier, shaderIdentifier);
	glDeleteShader(shaderIdentifier);
}

int main(int /*argc*/, char** /*argv*/)
{
	glfwSetErrorCallback(ErrorCallback);

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

	const GLuint programIdentifier = glCreateProgram();

	const std::string vertexShader = ReadFile("Data/VertexShader.txt");
	const std::string fragmentShader = ReadFile("Data/FragmentShader.txt");

	AttachShader(programIdentifier, GL_VERTEX_SHADER, vertexShader.c_str());
	AttachShader(programIdentifier, GL_FRAGMENT_SHADER, fragmentShader.c_str());
	glLinkProgram(programIdentifier);
	CheckShaderLinkStatus(programIdentifier);

	constexpr float vertices[] =
	{
		-0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		-0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f
	};

	const unsigned int indices[] =
	{
		0, 1, 2,
		2, 3, 0
	};

	unsigned int vertexBufferObject = 0;
	unsigned int vertexArrayObject = 0;
	unsigned int elementBufferObject = 0;
	glGenVertexArrays(1, &vertexBufferObject);
	glGenBuffers(1, &vertexArrayObject);
	glGenBuffers(1, &elementBufferObject);

	glBindVertexArray(vertexBufferObject);

	glBindBuffer(GL_ARRAY_BUFFER, vertexArrayObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), static_cast<void*>(nullptr));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	const Tilemap tilemap = ParseTilemap();

	int textureIdentifier = -1;
	if (!tilemap.myImage.myFilepath.empty())
		textureIdentifier = CreateTexture(tilemap.myImage.myFilepath);

	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(programIdentifier);

		if (textureIdentifier != -1)
		{
			glActiveTexture(GL_TEXTURE0 + textureIdentifier);
			glUniform1i(glGetUniformLocation(programIdentifier, "ourTexture"), textureIdentifier);
			glBindTexture(GL_TEXTURE_2D, textureIdentifier);
		}

		glBindVertexArray(vertexArrayObject);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}
