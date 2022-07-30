#pragma once

#include <string>

class Shader
{
public:
	Shader(const std::string& aFilepath);
	~Shader();

	void AttachShader(unsigned int aProgramIdentifier, unsigned int aType, const char* aSource);
	void CheckShaderLinkStatus(unsigned int aProgramIdentifier) const;

	[[nodiscard]] std::string GetFilepath() const { return myFilepath; }

private:
	void CheckShaderCompileStatus(unsigned int aShaderIdentifier) const;

	std::string myFilepath;
	unsigned int myShaderIdentifier;
	unsigned int myShaderType;
};
