#pragma once
#include "Chunk.h"


namespace Voxel {
	struct ChunkNeighbors;
	class ChunkRefHandle;

	class ChunkLightEngine {
	private:
		union {
			struct {
				Light backLeft[CHUNK_VOLUME];
				Light back[CHUNK_VOLUME];
				Light backRight[CHUNK_VOLUME];
				
				Light left[CHUNK_VOLUME];
				Light middle[CHUNK_VOLUME];
				Light right[CHUNK_VOLUME];
				
				Light frontLeft[CHUNK_VOLUME];
				Light front[CHUNK_VOLUME];
				Light frontRight[CHUNK_VOLUME];
			};
			
			Light lightArray[CHUNK_VOLUME][9];
		};

		std::deque<Chunk::LightNode> lightQueue[3][3];

	public:
		ChunkLightEngine();
		~ChunkLightEngine();

		void loadLightData(const ChunkNeighbors &);
		void loadLightDataAsync(const ChunkNeighbors &);

		void computeChunkLighting(ChunkNeighbors &);

	private:
		typedef Chunk::LightNode Node;
		typedef std::deque<Node> Queue;

		void propagateLight(int chunkX, int chunkZ, const ChunkRefHandle &);

		inline constexpr Queue& getQueue(int cx, int cz) {
			assert(cx >= -1 && cx <= 1);
			assert(cz >= -1 && cz <= 1);
			return lightQueue[cz + 1][cx + 1];
		}

		inline constexpr Light* getLightArray(int cx, int cz) {
			assert(cx >= -1 && cx <= 1);
			assert(cz >= -1 && cz <= 1);
			cx += 1;
			cz += 1;
			return *lightArray + (CHUNK_VOLUME * (cx + cz * 3));
		}

		inline bool canPropagateLight(int x, int y, int z, Light light, Light *data, const ChunkRefHandle &handle);

	};
}

