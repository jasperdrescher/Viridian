#include "MapLayer.hpp"

#include <glad/glad.h>
#include <tmxlite/TileLayer.hpp>

MapLayer::MapLayer(const tmx::Map& aMap, std::size_t aLayerIndex, const std::vector<unsigned int>& aTextureIdentifier)
	: myTilesetTextureIdentifiers(aTextureIdentifier)
{
	CreateSubsets(aMap, aLayerIndex);
}

MapLayer::~MapLayer()
{
	for (Subset& subset : mySubsets)
	{
		if (subset.myVertexBufferObject)
			glDeleteBuffers(1, &subset.myVertexBufferObject);

		if (subset.myLookup)
			glDeleteTextures(1, &subset.myLookup);
	}
}

void MapLayer::Draw() const
{
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));

	for (const Subset& subset : mySubsets)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, subset.myTextureIdentifier);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, subset.myLookup);

		glBindBuffer(GL_ARRAY_BUFFER, subset.myVertexBufferObject);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

MapLayer::Subset::Subset()
	: myVertexBufferObject(0)
	, myTextureIdentifier(0)
	, myLookup(0)
{}

void MapLayer::CreateSubsets(const tmx::Map& aMap, std::size_t aLayerIndex)
{
	const std::vector<tmx::Layer::Ptr>& layers = aMap.getLayers();
	if (aLayerIndex >= layers.size() || (layers[aLayerIndex]->getType() != tmx::Layer::Type::Tile))
	{
		printf("Invalid layer index or layer type, layer will be empty\n");
		return;
	}

	const tmx::TileLayer* const layer = dynamic_cast<const tmx::TileLayer*>(layers[aLayerIndex].get());

	const tmx::FloatRect bounds = aMap.getBounds();
	const float verts[] =
	{
		bounds.left, bounds.top, 0.0f, 0.0f, 0.0f,
		bounds.left + bounds.width, bounds.top, 0.0f, 1.0f, 0.0f,
		bounds.left, bounds.top + bounds.height, 0.0f, 0.0f, 1.0f,
		bounds.left + bounds.width, bounds.top + bounds.height, 0.0f, 1.0f, 1.0f
	};

	const tmx::Vector2u& mapSize = aMap.getTileCount();
	const std::vector<tmx::Tileset>& tilesets = aMap.getTilesets();
	for (unsigned int i = 0; i < tilesets.size(); ++i)
	{
		const tmx::Tileset& tileset = tilesets[i];
		const std::vector<tmx::TileLayer::Tile>& tileIDs = layer->getTiles();
		std::vector<std::uint16_t> pixelData;
		bool tsUsed = false;

		for (unsigned int y = 0; y < mapSize.y; ++y)
		{
			for (unsigned int x = 0; x < mapSize.x; ++x)
			{
				const unsigned index = y * mapSize.x + x;
				if (index < tileIDs.size()
					&& tileIDs[index].ID >= tileset.getFirstGID()
					&& tileIDs[index].ID < (tileset.getFirstGID() + tileset.getTileCount()))
				{
					pixelData.push_back(static_cast<std::uint16_t>((tileIDs[index].ID - tileset.getFirstGID()) + 1)); // Red channel - making sure to index relative to the tileset
					pixelData.push_back(tileIDs[index].flipFlags); // Green channel - tile flips are performed on the shader
					tsUsed = true;
				}
				else
				{
					// Pad with empty space
					pixelData.push_back(0);
					pixelData.push_back(0);
				}
			}
		}

		// If we have some data for this tile set, create the resources
		if (tsUsed)
		{
			mySubsets.emplace_back();
			mySubsets.back().myTextureIdentifier = myTilesetTextureIdentifiers[i];

			glGenBuffers(1, &mySubsets.back().myVertexBufferObject);
			glBindBuffer(GL_ARRAY_BUFFER, mySubsets.back().myVertexBufferObject);
			glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

			glGenTextures(1, &mySubsets.back().myLookup);
			glBindTexture(GL_TEXTURE_2D, mySubsets.back().myLookup);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16UI, static_cast<GLsizei>(mapSize.x), static_cast<GLsizei>(mapSize.y), 0, GL_RG_INTEGER, GL_UNSIGNED_SHORT, static_cast<void*>(pixelData.data()));

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}
	}
}
