#include "Shader.hpp"

#include <cstdio>
#include <glad/glad.h>

Shader::Shader(const std::string& aFilepath)
	: myFilepath(aFilepath)
	, myShaderIdentifier(0)
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
	if (isLinked == GL_FALSE)
	{
		char infoLog[512];
		glGetProgramInfoLog(aProgramIdentifier, 512, nullptr, infoLog);
		printf("Failed to link shader %s: %s\n", myFilepath.c_str(), infoLog);
	}
}

void Shader::CheckShaderCompileStatus(unsigned int aShaderIdentifier) const
{
	int isCompiled = GL_FALSE;
	glGetShaderiv(aShaderIdentifier, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		char shaderInfoLog[512];
		glGetShaderInfoLog(aShaderIdentifier, 512, nullptr, shaderInfoLog);
		printf("Failed to compile shader %s: %s\n", myFilepath.c_str(), shaderInfoLog);
	}
}
