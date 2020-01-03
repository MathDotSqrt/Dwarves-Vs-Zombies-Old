#pragma once
#include "System.h"
#include <unordered_map>

namespace SLNet{
	struct Packet;
	typedef unsigned char MessageID;
}

class NetWorkSystem : public System {
private:
	std::unordered_map<entt::entity, entt::entity> netToClientMap;

	void recieveNetID(Engine *engine, SLNet::Packet *packet);
	void updatePlayer(Engine *engine, SLNet::Packet *packet);
	SLNet::MessageID getPacketID(SLNet::Packet *packet);
public:
	NetWorkSystem(int priority);
	~NetWorkSystem();

	void addedToEngine(Engine * engine);
	void removedFromEngine(Engine * engine);
	void update(Engine * engine, float delta);
};

