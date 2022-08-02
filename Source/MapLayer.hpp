#pragma once

#include <tmxlite/Map.hpp>

#include <vector>

class MapLayer final
{
public:
	MapLayer(const tmx::Map& aMap, std::size_t aLayerIndex, const std::vector<unsigned>& aTextureIdentifier);
	~MapLayer();

	MapLayer(const MapLayer&) = delete;
	MapLayer& operator=(const MapLayer&) = delete;

	void Draw() const;

private:
	struct Subset
	{
		Subset();

		unsigned int myVertexBufferObject;
		unsigned int myTextureIdentifier;
		unsigned int myLookup;
	};

	void CreateSubsets(const tmx::Map& aMap, std::size_t aLayerIndex);

	std::vector<Subset> mySubsets;
	const std::vector<unsigned int>& myTilesetTextureIdentifiers;
};
