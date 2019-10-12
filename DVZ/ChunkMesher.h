#pragma once
#include "Chunk.h"
#include "Geometry.h"

namespace Voxel{

class ChunkMesher {
private:
	//Block block[];

public:
	ChunkMesher();
	~ChunkMesher();

	void loadChunkData(Chunk *chunk, Chunk *up, Chunk *down, Chunk *left, Chunk *right, Chunk *front, Chunk *back);
	void loadChunkDataAsync(Chunk *chunk, Chunk *up, Chunk *down, Chunk *left, Chunk *right, Chunk *front, Chunk *back);

	void createChunkMesh(Chunk::BlockGeometry *geometry);
};
}

