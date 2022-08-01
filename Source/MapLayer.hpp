#pragma once

#include <vector>

namespace tmx
{
    class Map;
}

class MapLayer final
{
public:
    MapLayer(const tmx::Map&, std::size_t, const std::vector<unsigned>&);
    ~MapLayer();
    
    MapLayer(const MapLayer&) = delete;
    MapLayer& operator = (const MapLayer&) = delete;

    void draw();

private:

    const std::vector<unsigned>& m_tilesetTextures;

    struct Subset final
    {
        unsigned vbo = 0;
        unsigned texture = 0;
        unsigned lookup = 0;
    };
    std::vector<Subset> m_subsets;

    void createSubsets(const tmx::Map&, std::size_t);
};
