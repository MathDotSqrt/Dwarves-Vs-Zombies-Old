#pragma once
#include "Chunk.h"
#include "ChunkGeometry.h"
#include "Geometry.h"

namespace Voxel {

	struct ChunkNeighbors;


	class ChunkMesher {
	private:
		const static int PADDED_WIDTH_X = Voxel::CHUNK_WIDTH_X + 2;
		const static int PADDED_WIDTH_Y = Voxel::CHUNK_WIDTH_Y + 2;
		const static int PADDED_WIDTH_Z = Voxel::CHUNK_WIDTH_Z + 2;
		const static int PADDED_VOLUME = PADDED_WIDTH_X * PADDED_WIDTH_Y * PADDED_WIDTH_Z;
		Block block[PADDED_VOLUME];

		typedef struct _BlockFaceCullTags {
			bool px;
			bool nx;
			bool py;
			bool ny;
			bool pz;
			bool nz;
		} BlockFaceCullTags;

		typedef ChunkGeometry::BlockVertex BlockVertex;

	public:
		ChunkMesher();
		~ChunkMesher();

		void loadChunkData(const ChunkNeighbors &);
		void loadChunkDataAsync(const ChunkNeighbors &);

		void createChunkMesh(ChunkGeometry *);

	private:
		void createCulledCube(int x, int y, int z, BlockFaceCullTags cull, Block b, ChunkGeometry *geometry);
		void createX(int x, int y, int z, Block b, ChunkGeometry *geometry);

		Block getBlock(int x, int y, int z);
		int toPaddedBlockIndex(int x, int y, int z);
	};
}
