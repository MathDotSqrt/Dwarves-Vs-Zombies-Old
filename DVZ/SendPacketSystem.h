#pragma once
#include "System.h"

class SendPacketSystem : public System{
public:
	SendPacketSystem(int priority);
	~SendPacketSystem();

	void addedToEngine(Engine * engine);
	void removedFromEngine(Engine * engine);
	void update(Engine * engine, float delta);
};

