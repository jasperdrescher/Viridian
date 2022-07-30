#include "Texture.hpp"

#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Texture::Texture()
	: myTextureIdentifier(0)
	, myReferenceCount(0)
{}

Texture::~Texture()
{
	glDeleteTextures(1, &myTextureIdentifier);
}

void Texture::LoadTexture(const std::string& aFilepath)
{
	stbi_set_flip_vertically_on_load(1);

	int width = 0;
	int height = 0;
	int numberOfChannels = 0;
	unsigned char* data = stbi_load(aFilepath.c_str(), &width, &height, &numberOfChannels, 3);
	if (!data)
	{
		printf("Failed to load %s\n", aFilepath.c_str());
		return;
	}

	glGenTextures(1, &myTextureIdentifier);
	glBindTexture(GL_TEXTURE_2D, myTextureIdentifier);
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glGenerateMipmap(GL_TEXTURE_2D);

	//const GLint internalformat = numberOfChannels == 3 ? GL_RGB8 : GL_RGBA8;
	//const GLenum format = numberOfChannels == 3 ? GL_RGB : GL_RGBA;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(data);
}

void Texture::IncRefCount()
{
	myReferenceCount++;
}

void Texture::DecRefCount()
{
	myReferenceCount--;
	if (myReferenceCount == 0)
		delete this;
}

unsigned int Texture::GetTextureIdentifier() const
{
	return myTextureIdentifier;
}
