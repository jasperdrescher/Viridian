#pragma once

#include <string>

class Texture
{
public:
    Texture(unsigned& aTextureIdentifier);
    ~Texture();

    void LoadTexture(const std::string& aFilepath);
    void IncRefCount();
    void DecRefCount();
    [[nodiscard]] unsigned int GetTextureIdentifier() const;

private:
    unsigned& myTextureIdentifier;
    unsigned int myReferenceCount;
};
