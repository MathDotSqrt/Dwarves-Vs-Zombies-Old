#pragma once
#include "System.h"
#include "entt.hpp"
#include "Scene.h"

namespace SLNet{
	class RakPeerInterface;
	struct Packet;
	typedef unsigned char MessageID;
}

class NetWorkSystem : public System {
private:
	std::unordered_map<unsigned int, unsigned int> netToEnttID;

	Graphics::Scene *scene;
	SLNet::RakPeerInterface *peer;


	bool isConnected = false;
	entt::entity clientEntt = 0;

	SLNet::MessageID getPacketID(SLNet::Packet *packet);
public:
	NetWorkSystem(int priority, entt::entity clientEntt, Graphics::Scene *scene);
	~NetWorkSystem();

	void addedToEngine(Engine * engine);
	void removedFromEngine(Engine * engine);
	void update(Engine * engine, float delta);
};

