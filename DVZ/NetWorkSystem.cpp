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

	
}

void NetWorkSystem::removedFromEngine(Engine * engine) {
}

void NetWorkSystem::update(Engine * engine, float delta) {
	RakPeerInterface *peer = engine->getPeer();
	
	Packet *packet;
	for (packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive()) {
		MessageID id = getPacketID(packet);

		switch (id) {
		case ID_SPAWN_ENTITY:
			LOG_NET("SPAWN: %d", id);
			break;
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