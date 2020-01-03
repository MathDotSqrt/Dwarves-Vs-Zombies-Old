#include "NetWorkSystem.h"
#include "Engine.h"
#include "Components.h"
#include "macrologger.h"

#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"
#include "BitStream.h"
#include "GamePacketID.h"

#include "Scene.h"

using namespace SLNet;

NetWorkSystem::NetWorkSystem(int priority) : System(priority) {
}

NetWorkSystem::~NetWorkSystem() {

}

void NetWorkSystem::addedToEngine(Engine * engine) {
	LOG_SYSTEM("Added to engine");

	//engine->group<PrototypeComponent>();
}

void NetWorkSystem::removedFromEngine(Engine * engine) {
}


void NetWorkSystem::update(Engine * engine, float delta) {
	RakPeerInterface *peer = engine->getPeer();
	
	entt::entity id = engine->getPlayer();

	Packet *packet;
	for (packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive()) {
		MessageID id = getPacketID(packet);


		switch (id) {
		case ID_CONNECTION_REQUEST_ACCEPTED:
			LOG_NET("CONNECTION ACCEPTED");
			break;
		case ID_PLAYER_MOVE:
			updatePlayer(engine, packet);
			break;
		case ID_CLIENT_NET_ID:
			recieveNetID(engine, packet);
			break;
		default:
			LOG_NET("ID CAUGHT: %d", id);
		}
	}
}

void NetWorkSystem::recieveNetID(Engine *engine, Packet *packet) {
	BitStream stream(packet->data, packet->length, false);

	stream.IgnoreBytes(sizeof(MessageID));
	entt::entity playerID = engine->getPlayer();
	entt::entity netID = entt::null;
	stream.Read(netID);
	engine->assign_or_replace<NetworkComponent>(playerID, netID);
	netToClientMap.erase(netID);
	netToClientMap[netID] = playerID;
	LOG_NET("CLIENT ID RECIEVED: %d", netID);
}


void NetWorkSystem::updatePlayer(Engine *engine, Packet *packet) {
	BitStream read(packet->data, packet->length, false);

	read.IgnoreBytes(sizeof(MessageID));
	
	entt::entity playerID = entt::null;
	entt::entity netID = entt::null;
	glm::vec3 pos;
	read.Read(netID);
	read.Read(pos);

	auto iter = netToClientMap.find(netID);
	if (iter == netToClientMap.end()) {
		playerID = engine->create();
		engine->assign<PositionComponent>(playerID, pos);
		engine->assign<RotationComponent>(playerID, glm::quat(glm::vec3(0, 0, 0)));
		engine->assign<ScaleComponent>(playerID, glm::vec3(1, 1, 1));

		netToClientMap[netID] = playerID;
		LOG_NET("NEW ENT");
	}
	else {
		playerID = iter->second;
		auto &position = engine->get<PositionComponent>(playerID);
		position.pos = pos;
	}
}

MessageID NetWorkSystem::getPacketID(Packet *packet) {
	MessageID id = (MessageID)packet->data[0];

	if (id == ID_TIMESTAMP) {
		return (MessageID)packet->data[sizeof(MessageID) + sizeof(Time)];
	}
	else {
		return (MessageID)packet->data[0];
	}
}