#pragma once
#include "Chunk.h"
#include "ChunkGeometry.h"
#include "Geometry.h"

namespace Voxel {

	struct ChunkNeighbors;


	class ChunkMesher {
	private:
		constexpr static int PADDED_WIDTH_X = Voxel::CHUNK_WIDTH_X + 2;
		constexpr static int PADDED_WIDTH_Y = Voxel::CHUNK_WIDTH_Y + 2;
		constexpr static int PADDED_WIDTH_Z = Voxel::CHUNK_WIDTH_Z + 2;
		constexpr static int PADDED_VOLUME = PADDED_WIDTH_X * PADDED_WIDTH_Y * PADDED_WIDTH_Z;
		Block blockData[PADDED_VOLUME];
		
		constexpr static int CHUNK_VOLUME = Voxel::CHUNK_WIDTH_X * Voxel::CHUNK_WIDTH_Y * Voxel::CHUNK_WIDTH_Z;
		Light lightData[PADDED_VOLUME];


		typedef struct _BlockFaceCullTags {
			bool px;
			bool nx;
			bool py;
			bool ny;
			bool pz;
			bool nz;
		} BlockFaceCullTags;

		struct LightFace {
			Light px;
			Light nx;
			Light py;
			Light ny;
			Light pz;
			Light nz;
		};

		typedef ChunkGeometry::BlockVertex BlockVertex;

	public:
		ChunkMesher();
		~ChunkMesher();

		void loadChunkData(const ChunkNeighbors &);
		void loadChunkDataAsync(const ChunkNeighbors &);

		void createChunkMesh(ChunkGeometry *);

	private:
		void createCulledCube(int x, int y, int z, Block b, LightFace l, BlockFaceCullTags cull, ChunkGeometry *geometry);
		void createX(int x, int y, int z, Block b, Light l, ChunkGeometry *geometry);

		Block getBlock(int x, int y, int z);
		Light getLight(int x, int y, int z);
		int toPaddedBlockIndex(int x, int y, int z);
	};
}
