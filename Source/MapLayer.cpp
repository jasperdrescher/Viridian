#include "MapLayer.hpp"

#include <glad/glad.h>
#include <tmxlite/TileLayer.hpp>

MapLayer::MapLayer(const tmx::Map& map, std::size_t layerIdx, const std::vector<unsigned>& textures)
	: myTilesetTextureIdentifiers(textures)
{
	CreateSubsets(map, layerIdx);
}

MapLayer::~MapLayer()
{
	for (auto& ss : mySubsets)
	{
		if (ss.myVertexBufferObject)
			glDeleteBuffers(1, &ss.myVertexBufferObject);

		if (ss.myLookup)
			glDeleteTextures(1, &ss.myLookup);
	}
}

void MapLayer::Draw() const
{
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));

	for (const auto& ss : mySubsets)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, ss.myTextureIdentifier);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, ss.myLookup);

		glBindBuffer(GL_ARRAY_BUFFER, ss.myVertexBufferObject);
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
	const auto& layers = aMap.getLayers();
	if (aLayerIndex >= layers.size() || (layers[aLayerIndex]->getType() != tmx::Layer::Type::Tile))
	{
		printf("Invalid layer index or layer type, layer will be empty\n");
		return;
	}

	const auto layer = dynamic_cast<const tmx::TileLayer*>(layers[aLayerIndex].get());

	const auto bounds = aMap.getBounds();
	const float verts[] =
	{
		bounds.left, bounds.top, 0.f, 0.f, 0.f,
		bounds.left + bounds.width, bounds.top, 0.f, 1.f, 0.f,
		bounds.left, bounds.top + bounds.height, 0.f, 0.f, 1.f,
		bounds.left + bounds.width, bounds.top + bounds.height, 0.f, 1.f, 1.f
	};

	const auto& mapSize = aMap.getTileCount();
	const auto& tilesets = aMap.getTilesets();
	for (auto i = 0u; i < tilesets.size(); ++i)
	{
		//check each tile ID to see if it falls in the current tile set
		const auto& ts = tilesets[i];
		const auto& tileIDs = layer->getTiles();
		std::vector<std::uint16_t> pixelData;
		bool tsUsed = false;

		for (auto y = 0u; y < mapSize.y; ++y)
		{
			for (auto x = 0u; x < mapSize.x; ++x)
			{
				const auto idx = y * mapSize.x + x;
				if (idx < tileIDs.size() && tileIDs[idx].ID >= ts.getFirstGID()
					&& tileIDs[idx].ID < (ts.getFirstGID() + ts.getTileCount()))
				{
					pixelData.push_back(static_cast<std::uint16_t>((tileIDs[idx].ID - ts.getFirstGID()) + 1)); //red channel - making sure to index relative to the tileset
					pixelData.push_back(tileIDs[idx].flipFlags); //green channel - tile flips are performed on the shader
					tsUsed = true;
				}
				else
				{
					//pad with empty space
					pixelData.push_back(0);
					pixelData.push_back(0);
				}
			}
		}

		//if we have some data for this tile set, create the resources
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
