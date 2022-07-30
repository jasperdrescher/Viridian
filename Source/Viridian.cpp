#include "FileUtility.hpp"
#include "GLDebugUtility.hpp"

#include <GLFW/glfw3.h>
#include <tileson.hpp>

#include "SpriteBatcher.hpp"
#include "Texture.hpp"

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
	const std::string tilemapFilepath = FileUtility::ReadFile(settingsSource);
	const std::string dataString = FileUtility::ReadFile(tilemapFilepath.c_str());
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

void PrintDebugInfo()
{
	printf("OpenGL %s\n", glGetString(GL_VERSION));
	printf("GLSL %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	printf("Vendor %s\n", glGetString(GL_VENDOR));
	printf("Renderer %s\n", glGetString(GL_RENDERER));
	int major, minor, revision;
	glfwGetVersion(&major, &minor, &revision);
	printf("GLFW %d.%d.%d\n", major, minor, revision);
}

void ErrorCallback(int anError, const char* aDescription)
{
	printf("%i %s", anError, aDescription);
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
	glDebugMessageCallback(GLDebugUtility::ErrorCallback, nullptr);

	const Tilemap tilemap = ParseTilemap();

	Texture* texture = new Texture();
	if (!tilemap.myImage.myFilepath.empty())
	{
		texture->LoadTexture(tilemap.myImage.myFilepath);
		texture->IncRefCount();
	}

	SpriteBatcher spriteBatcher(glm::vec2(640, 480));

	const Tile& firstTile = tilemap.myTiles[5];
	printf("%0.1f %0.1f %i %i %i %i\n", firstTile.myPosition.x, firstTile.myPosition.y, firstTile.myDrawingRectangle.x, firstTile.myDrawingRectangle.y, firstTile.myDrawingRectangle.width, firstTile.myDrawingRectangle.height);

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

		/*const Tile& firstTile = tilemap.myTiles[5];
		spriteBatcher.Draw(glm::vec4(firstTile.myPosition.x, firstTile.myPosition.y, firstTile.myDrawingRectangle.width, firstTile.myDrawingRectangle.height),
				glm::vec4(firstTile.myDrawingRectangle.x, firstTile.myDrawingRectangle.y, firstTile.myDrawingRectangle.width, firstTile.myDrawingRectangle.height),
			glm::vec4(1.0f),
			texture);*/

		for (const Tile& tile : tilemap.myTiles)
		{
			spriteBatcher.Draw(glm::vec4(tile.myPosition.x, tile.myPosition.y, tile.myDrawingRectangle.width, tile.myDrawingRectangle.height),
				glm::vec4(tile.myDrawingRectangle.x, tile.myDrawingRectangle.y, tile.myDrawingRectangle.width, tile.myDrawingRectangle.height), glm::vec4(1.0f), texture);
		}

		spriteBatcher.Flush();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	delete texture;

	glfwTerminate();

	return 0;
}
