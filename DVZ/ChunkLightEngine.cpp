#include "ChunkLightEngine.h"
#include "ChunkManager.h"


using namespace Voxel;

ChunkLightEngine::ChunkLightEngine() {
}

ChunkLightEngine::~ChunkLightEngine() {
}

void ChunkLightEngine::loadLightData(const ChunkNeighbors &chunks) {
	for (int z = -1; z <= 1; z++) {
		for (int x = -1; x <= 1; x++) {
			memcpy(getLightArray(x, z), chunks.getChunk(x, z)->lightData, sizeof(middle));
		}
	}

	std::lock_guard<std::shared_mutex> lock(chunks.middle->chunkMutex);
	getQueue(0, 0) = std::move(chunks.middle->lightQueue);
	chunks.middle->lightQueue.clear();
}

void ChunkLightEngine::computeChunkLighting(ChunkNeighbors &chunks) {

	int chunkX = 0;
	int chunkZ = 0;


	//{
	//	std::shared_lock<std::shared_mutex> lock(chunks.getChunk(0, 0)->chunkMutex);
	//	propagateLight(0, 0, chunks.getChunk(0, 0));
	//}

	bool allEmpty = true;
	do {
		allEmpty = true;
		for (int z = -1; z <= 1; z++) {
			for (int x = -1; x <= 1; x++) {
				if (getQueue(x, z).size()) {
					allEmpty = false;
					std::shared_lock<std::shared_mutex> lock(chunks.getChunk(x, z)->chunkMutex);
					propagateLight(x, z, chunks.getChunk(x, z));
				}
			}
		}

	} while (!allEmpty);


	for (int z = -1; z <= 1; z++) {
		for (int x = -1; x <= 1; x++) {
			std::lock_guard<std::shared_mutex> lock(chunks.getChunk(x, z)->chunkMutex);
			memcpy(chunks.getChunk(x, z)->lightData, getLightArray(x, z), sizeof(middle));
		}
	}
}

void ChunkLightEngine::propagateLight(int chunkX, int chunkZ, const ChunkRefHandle &chunk) {
	constexpr int MIN_X = 0;
	constexpr int MIN_Y = 0;
	constexpr int MIN_Z = 0;
	constexpr int MAX_X = CHUNK_WIDTH_X - 1;
	constexpr int MAX_Y = CHUNK_WIDTH_Y - 1;
	constexpr int MAX_Z = CHUNK_WIDTH_Z - 1;
	
	Queue &queue = getQueue(chunkX, chunkZ);
	Light *lightData = getLightArray(chunkX, chunkZ);
	while (queue.size()) {
		Node node = queue.front();
		queue.pop_front();

		Light light = node.light;
		uint8 blockLight = light.getBlockLight();
		Light newLight(0, blockLight - 1);

		int x = node.x;
		int y = node.y;
		int z = node.z;
		int index = Chunk::toIndex(x, y, z);

		BlockType type = chunk->getBlockInternal(x, y, z).type;
		Light currentLight = lightData[index];

		if (type != BlockType::BLOCK_TYPE_DEFAULT || currentLight.getBlockLight() >= blockLight)
			continue;

		lightData[index].setBlockLight(blockLight);

		//right
		if (x == MAX_X) {
			getQueue(chunkX + 1, chunkZ).push_back({ newLight, MIN_X, y, z });
		}
		else if (canPropagateLight(x + 1, y, z, newLight, lightData, chunk)) {
			queue.push_back({ newLight, x + 1, y, z });
		}

		//left
		if (x == MIN_X) {
			getQueue(chunkX - 1, chunkZ).push_back({ newLight, MAX_X, y, z });
		}
		else if(canPropagateLight(x - 1, y, z, newLight, lightData, chunk)){
			queue.push_back({newLight, x - 1, y, z});
		}

		//up
		if (z == MAX_Z) {
			getQueue(chunkX, chunkZ + 1).push_back({ newLight, x, y, MIN_Z });
		}
		else if (canPropagateLight(x, y, z + 1, newLight, lightData, chunk)) {
			queue.push_back({ newLight, x, y, z + 1 });
		}

		//down
		if (z == MIN_Z) {
			getQueue(chunkX, chunkZ - 1).push_back({ newLight, x, y, MAX_Z });
		}
		else if (canPropagateLight(x, y, z - 1, newLight, lightData, chunk)) {
			queue.push_back({ newLight, x, y, z - 1 });
		}

	}
}

bool ChunkLightEngine::canPropagateLight(int x, int y, int z, Light light, Light *data, const ChunkRefHandle &chunk) {
	return chunk->getBlockInternal(x, y, z).type == BlockType::BLOCK_TYPE_DEFAULT
		&& data[Chunk::toIndex(x, y, z)].getBlockLight() < light.getBlockLight();
}
//void ChunkLightEngine::computeChunkLightingAsync(ChunkNeighbors &chunks) {
//
//}
