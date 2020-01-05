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
	
}

void NetWorkSystem::recieveNetID(Engine *engine, Packet *packet) {
	
}


void NetWorkSystem::updatePlayer(Engine *engine, Packet *packet) {
	
}

