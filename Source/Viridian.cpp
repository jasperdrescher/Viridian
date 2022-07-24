#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <tileson.hpp>

#include <filesystem>
#include <fstream>
#include <string>
#include <sstream>

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

	const char* source = "content/test-maps/demo-tileset.json";
	if (std::filesystem::exists(source))
	{
		std::ifstream fileStream(source, std::ifstream::in);
		assert(fileStream.is_open());
		if (fileStream.is_open())
		{
			std::stringstream sourceStringStream;
			sourceStringStream << fileStream.rdbuf();

			fileStream.close();

			const std::string dataString = sourceStringStream.str();
			const unsigned int bufferSize = static_cast<unsigned int>(dataString.length()) + 1;
			char* buffer = new char[bufferSize];
			strcpy_s(buffer, bufferSize, dataString.c_str());

			tson::Tileson t;
			std::unique_ptr<tson::Map> map = t.parse(buffer, bufferSize);
		}
	}

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}
