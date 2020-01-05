#include "SendPacketSystem.h"
#include "Components.h"
#include "Engine.h"

#include "RakPeerInterface.h"
#include "BitStream.h"
#include "MessageIdentifiers.h"
#include "GamePacketID.h"

SendPacketSystem::SendPacketSystem(int priority) : System(priority) {

}

SendPacketSystem::~SendPacketSystem() {

}

void SendPacketSystem::addedToEngine(Engine *engine) {

}
void SendPacketSystem::removedFromEngine(Engine *engine) {

}
void SendPacketSystem::update(Engine *engine, float delta) {
	
}