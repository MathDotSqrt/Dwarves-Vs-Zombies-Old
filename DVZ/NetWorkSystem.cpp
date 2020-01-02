#include "NetWorkSystem.h"
#include "Engine.h"
#include "Components.h"
#include "macrologger.h"

#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"
#include "BitStream.h"
#include "GamePacketID.h"

#include "Scene.h"

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

}
