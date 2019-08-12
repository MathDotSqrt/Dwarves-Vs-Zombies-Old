#pragma once
#include "System.h"
class VoxelSystem : public System {

private:
	inline static int RENDER_DISTANCE = 10;

public:
	VoxelSystem(int priority);
	~VoxelSystem();

	void addedToEngine(Engine * engine);
	void removedFromEngine(Engine * engine);
	void update(Engine * engine, float delta);
};

