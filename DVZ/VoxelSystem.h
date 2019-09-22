#pragma once
#include "System.h"
#include <thread>
#include "ConcurrentQueue.h"


namespace Voxel{
	class Chunk;
}

class VoxelSystem : public System {

private:
	inline static int RENDER_DISTANCE = 10;
	//std::thread chunkLoaderThread;	//todo remove this

	bool shouldLoadChunks = true;

public:
	VoxelSystem(int priority);
	~VoxelSystem();

	void addedToEngine(Engine * engine);
	void removedFromEngine(Engine * engine);
	void update(Engine * engine, float delta);

	void chunkLoader();

	void loadChunk(Engine *engine, int cx, int cy, int cz);
};

