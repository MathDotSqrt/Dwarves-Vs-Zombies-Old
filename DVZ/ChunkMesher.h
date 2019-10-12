#pragma once
#include "Chunk.h"
#include "Geometry.h"

namespace Voxel{

struct ChunkNeighbors;

class ChunkMesher {
private:
	const static int PADDED_VOLUME = (Voxel::CHUNK_WIDTH_X + 2) * (Voxel::CHUNK_WIDTH_Y + 2) * (Voxel::CHUNK_RENDER_WIDTH_Z + 2);
	Block block[PADDED_VOLUME];
	

public:
	ChunkMesher();
	~ChunkMesher();

	void loadChunkData(ChunkNeighbors &);
	void loadChunkDataAsync(ChunkNeighbors &);

	void createChunkMesh(Chunk::BlockGeometry &);
};
}

