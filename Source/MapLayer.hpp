#pragma once

#include <tmxlite/Map.hpp>

#include <vector>

class MapLayer final
{
public:
	MapLayer(const tmx::Map&, std::size_t, const std::vector<unsigned>&);
	~MapLayer();

	MapLayer(const MapLayer&) = delete;
	MapLayer& operator =(const MapLayer&) = delete;

	void Draw() const;

private:
	struct Subset
	{
		Subset();

		unsigned myVertexBufferObject;
		unsigned myTextureIdentifier;
		unsigned myLookup;
	};

	void CreateSubsets(const tmx::Map& aMap, std::size_t aLayerIndex);

	std::vector<Subset> mySubsets;
	const std::vector<unsigned>& myTilesetTextureIdentifiers;
};
