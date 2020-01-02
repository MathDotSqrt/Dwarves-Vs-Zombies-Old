#pragma once
#include "System.h"

namespace SLNet{
	struct Packet;
	typedef unsigned char MessageID;
}

class NetWorkSystem : public System {
private:
	SLNet::MessageID getPacketID(SLNet::Packet *packet);
public:
	NetWorkSystem(int priority);
	~NetWorkSystem();

	void addedToEngine(Engine * engine);
	void removedFromEngine(Engine * engine);
	void update(Engine * engine, float delta);
};

