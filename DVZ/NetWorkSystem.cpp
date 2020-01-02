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
	
	//LOG_NET("%zu", engine->group<PositionComponent>().size());
	entt::entity id = engine->getPlayer();

	Packet *packet;
	for (packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive()) {
		MessageID id = getPacketID(packet);

		BitStream stream(packet->data, packet->length, false);

		switch (id) {
		case ID_CONNECTION_REQUEST_ACCEPTED:
			LOG_NET("CONNECTION ACCEPTED");
			break;
		case ID_SPAWN_ENTITY:
			LOG_NET("SPAWN: %d", id);
			break;
		case ID_CLIENT_NET_ID:
			stream.IgnoreBytes(sizeof(MessageID));
			uint32 n;
			stream.Read(n);
			engine->assign_or_replace<NetworkComponent>(n);
			LOG_NET("CLIENT ID RECIEVED: %d", id);
		default:
			LOG_NET("ID CAUGHT: %d", id);
		}
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