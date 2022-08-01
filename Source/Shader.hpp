#pragma once

class Shader final
{
public:
	Shader();
	~Shader();

	void AttachShader(unsigned int aProgramIdentifier, unsigned int aType, const char* aSource);
	void CheckShaderLinkStatus(unsigned int aProgramIdentifier) const;

private:
	void CheckShaderCompileStatus(unsigned int aShaderIdentifier) const;

	unsigned int myShaderIdentifier;
	unsigned int myShaderType;
};
