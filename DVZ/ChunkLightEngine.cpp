#include "ChunkLightEngine.h"
#include "ChunkManager.h"


using namespace Voxel;

ChunkLightEngine::ChunkLightEngine() {
}

ChunkLightEngine::~ChunkLightEngine() {
}

void ChunkLightEngine::loadLightData(const ChunkNeighbors &chunks) {
	memcpy(middle, chunks.middle->lightData, sizeof(middle));
	memcpy(left, chunks.left->lightData, sizeof(left));
	memcpy(right, chunks.right->lightData, sizeof(right));
	memcpy(front, chunks.front->lightData, sizeof(front));
	memcpy(back, chunks.back->lightData, sizeof(back));
	memcpy(frontLeft, chunks.frontLeft->lightData, sizeof(frontLeft));
	memcpy(frontRight, chunks.frontRight->lightData, sizeof(frontRight));
	memcpy(backLeft, chunks.backLeft->lightData, sizeof(backLeft));
	memcpy(backRight, chunks.backRight->lightData, sizeof(backRight));


	std::lock_guard<std::shared_mutex> lock(chunks.middle->chunkMutex);
	getQueue(0, 0) = std::move(chunks.middle->lightQueue);
	chunks.middle->lightQueue.clear();
}

void ChunkLightEngine::computeChunkLighting(ChunkNeighbors &chunks) {
	typedef Chunk::LightNode Node;
	typedef std::deque<Node> Queue;
	
	Queue &q = getQueue(0, 0);
	Light *array = getLightArray(0, 0);
	while (q.size()) {
		Node node = q.front();
		array[Chunk::toIndex(node.x, node.y, node.z)] = node.value;
		q.pop_front();
	}
	
	{
		std::lock_guard<std::shared_mutex> lock(chunks.middle->chunkMutex);
		memcpy(chunks.middle->lightData, middle, sizeof(middle));
	}
}
//void ChunkLightEngine::computeChunkLightingAsync(ChunkNeighbors &chunks) {
//
//}
