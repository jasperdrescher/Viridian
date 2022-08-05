#pragma once

#include <glad/glad.h>
#include <cstdio>

namespace GLDebugUtility
{
	static void GLAPIENTRY ErrorCallback(GLenum aSource,
		GLenum aType,
		GLuint anID,
		GLenum aSeverity,
		GLsizei /*aLength*/,
		const GLchar* aMessage,
		const void* /*aUserParam*/)
	{
		std::string source;
		std::string type;
		std::string severity;

		switch (aSource)
		{
			case GL_DEBUG_SOURCE_API : source = "API"; break;
			case GL_DEBUG_SOURCE_WINDOW_SYSTEM : source = "WINDOW SYSTEM"; break;
			case GL_DEBUG_SOURCE_SHADER_COMPILER : source = "SHADER COMPILER"; break;
			case GL_DEBUG_SOURCE_THIRD_PARTY : source = "THIRD PARTY"; break;
			case GL_DEBUG_SOURCE_APPLICATION : source = "APPLICATION"; break;
			case GL_DEBUG_SOURCE_OTHER : source = "OTHER"; break;
			default : source = "UNKNOWN"; break;
		}

		switch (aType)
		{
			case GL_DEBUG_TYPE_ERROR : type = "ERROR"; break;
			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR : type = "DEPRECATED BEHAVIOR"; break;
			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR : type = "UDEFINED BEHAVIOR"; break;
			case GL_DEBUG_TYPE_PORTABILITY : type = "PORTABILITY"; break;
			case GL_DEBUG_TYPE_PERFORMANCE : type = "PERFORMANCE"; break;
			case GL_DEBUG_TYPE_OTHER : type = "OTHER"; break;
			case GL_DEBUG_TYPE_MARKER : type = "MARKER"; break;
			default : type = "UNKNOWN"; break;
		}

		switch (aSeverity)
		{
			case GL_DEBUG_SEVERITY_HIGH : severity = "HIGH"; break;
			case GL_DEBUG_SEVERITY_MEDIUM : severity = "MEDIUM"; break;
			case GL_DEBUG_SEVERITY_LOW : severity = "LOW"; break;
			case GL_DEBUG_SEVERITY_NOTIFICATION : severity = "NOTIFICATION"; break;
			default : severity = "UNKNOWN"; break;
		}

		printf("Message from OpenGL: Severity: %s Type: %s Source: %s Id: 0x%x\n", severity.c_str(), type.c_str(), source.c_str(), anID);
		printf("%s\n", aMessage);
	}
} // namespace GLDebugUtility
