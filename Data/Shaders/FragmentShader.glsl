#version 460

#define FLIP_HORIZONTAL 8u
#define FLIP_VERTICAL 4u
#define FLIP_DIAGONAL 2u

in vec2 vTextureCoordinates;

uniform usampler2D uLookupMap;
uniform sampler2D uTileMap;
uniform vec2 uTileSize = vec2(64.0);
uniform vec2 uTilesetCount = vec2(6.0, 7.0);
uniform vec2 uTilesetScale = vec2(1.0);
uniform float uOpacity = 1.0;

out vec4 colour;

/*fixes rounding imprecision on AMD cards*/
const float epsilon = 0.000005;

void main()
{
    uvec2 values = texture(uLookupMap, vTextureCoordinates).rg;
    if(values.r > 0u)
    {
        float index = float(values.r) - 1.0;
        vec2 position = vec2(mod(index + epsilon, uTilesetCount.x), floor((index / uTilesetCount.x) + epsilon)) / uTilesetCount;
        vec2 offsetCoord = (vTextureCoordinates * (textureSize(uLookupMap, 0) * uTilesetScale)) / uTileSize;

        vec2 texelSize = vec2(1.0) / textureSize(uLookupMap, 0);
        vec2 offset = mod(vTextureCoordinates, texelSize);
        vec2 ratio = offset / texelSize;
        offset = ratio * (1.0 / uTileSize);
        offset *= uTileSize / uTilesetCount;

        if (values.g != 0u)
        {
            vec2 tileSize = vec2(1.0) / uTilesetCount;
            if ((values.g & FLIP_DIAGONAL) != 0u)
            {
                float temp = offset.x;
                offset.x = offset.y;
                offset.y = temp;
                temp = tileSize.x / tileSize.y;
                offset.x *= temp;
                offset.y /= temp;
                offset.x = tileSize.x - offset.x;
                offset.y = tileSize.y - offset.y;
            }

            if ((values.g & FLIP_VERTICAL) != 0u)
            {
                offset.y = tileSize.y - offset.y;
            }

            if ((values.g & FLIP_HORIZONTAL) != 0u)
            {
                offset.x = tileSize.x - offset.x;
            }
        }
        colour = texture(uTileMap, position + offset);
        colour.a = min(colour.a, uOpacity);
    }
    else
    {
        colour = vec4(0.0);
    }
}