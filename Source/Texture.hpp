#pragma once

#include <string>

class Texture
{
public:
    Texture();
    ~Texture();

    void LoadTexture(const std::string& aFilepath);
    void IncRefCount();
    void DecRefCount();
    [[nodiscard]] unsigned int GetTextureIdentifier() const;

private:
    unsigned int myTextureIdentifier;
    unsigned int myReferenceCount;
};
