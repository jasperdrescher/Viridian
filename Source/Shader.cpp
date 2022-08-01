#include "Shader.hpp"

#include <cstdio>
#include <string>
#include <glad/glad.h>

Shader::Shader()
	: myShaderIdentifier(0)
	, myShaderType(0)
{}

Shader::~Shader()
{
	if (myShaderIdentifier != 0)
		glDeleteShader(myShaderIdentifier);
}

void Shader::AttachShader(unsigned int aProgramIdentifier, unsigned int aType, const char* aSource)
{
	myShaderType = aType;
	myShaderIdentifier = glCreateShader(aType);
	glShaderSource(myShaderIdentifier, 1, &aSource, nullptr);
	glCompileShader(myShaderIdentifier);
	CheckShaderCompileStatus(myShaderIdentifier);
	glAttachShader(aProgramIdentifier, myShaderIdentifier);
	glDeleteShader(myShaderIdentifier);
}

void Shader::CheckShaderLinkStatus(unsigned int aProgramIdentifier) const
{
	int isLinked = GL_FALSE;
	glGetProgramiv(aProgramIdentifier, GL_LINK_STATUS, &isLinked);
	int resultLength = 0;
	glGetShaderiv(aProgramIdentifier, GL_INFO_LOG_LENGTH, &resultLength);
	if (isLinked == GL_FALSE)
	{
		std::string infoLog;
		infoLog.resize(resultLength + 1);
		glGetShaderInfoLog(aProgramIdentifier, resultLength, nullptr, &infoLog[0]);
		printf("Failed to link shader of type %i: %s\n", myShaderType, infoLog.c_str());
	}
}

void Shader::CheckShaderCompileStatus(unsigned int aShaderIdentifier) const
{
	int isCompiled = GL_FALSE;
	glGetShaderiv(aShaderIdentifier, GL_COMPILE_STATUS, &isCompiled);
	int resultLength = 0;
	glGetShaderiv(aShaderIdentifier, GL_INFO_LOG_LENGTH, &resultLength);
	if (isCompiled == GL_FALSE)
	{
		std::string infoLog;
		infoLog.resize(resultLength + 1);
		glGetShaderInfoLog(aShaderIdentifier, resultLength, nullptr, &infoLog[0]);
		printf("Failed to compile shader of type %i: %s\n", myShaderType, infoLog.c_str());
		glDeleteShader(aShaderIdentifier);
	}
}
