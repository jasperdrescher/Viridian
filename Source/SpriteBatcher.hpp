#pragma once

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <glm/mat3x3.hpp>
#include <vector>

struct Vertex2dUVColor
{
    glm::vec2 m_position;
    glm::vec2 m_texCoord;
    glm::vec4 m_color;

    // Makes a 2d vertex with uc and color data.
    Vertex2dUVColor(glm::vec2 position, glm::vec2 texCoord, glm::vec4 color)
    {
        m_position = position;
        m_texCoord = texCoord;
        m_color = color;
    }
};

class Shader;
class Texture;

class SpriteBatcher
{
public:
    SpriteBatcher(glm::vec2 screenSize);
    ~SpriteBatcher();

    void Draw(glm::vec4 destRect, glm::vec4 sourceRect, glm::vec4 color, Texture* texture);
    void Flush();

    // Call this to tell the spritebatcher how many pixels wide/tall the window is. Setting to 1 will make the entire screen render 1 pixel
    void SetScreenSize(glm::vec2 screenSize);

private:
    std::vector<Vertex2dUVColor> m_vertexBuffer;
    glm::mat3 m_screenTransform;
    Shader* m_vertexShader;
    Shader* m_fragmentShader;
    Texture* m_texture;
    int m_textureUniform;
    int m_screenTransformUniform;
    unsigned int m_vbo;
    unsigned int m_shaderProgram;
};
