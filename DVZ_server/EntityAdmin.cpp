#include "EntityAdmin.h"
#include "RakPeerInterface.h"
#include "NetReader.h"
#include "GameLogic.h"
#include "SingletonComponents.h"

#define MAX_CONNECTIONS 10
#define SERVER_PORT 60000

EntityAdmin::EntityAdmin() : peer(SLNet::RakPeerInterface::GetInstance()){
	addSystemUpdateFunction(System::net_update);
	addSystemUpdateFunction(GameLogic::input_system);
	addSystemUpdateFunction(GameLogic::movement_system);
	addSystemUpdateFunction(System::net_broadcast);

	//peer->ApplyNetworkSimulator(0, 0, 0);

	SLNet::SocketDescriptor sd(SERVER_PORT, nullptr);
	peer->Startup(MAX_CONNECTIONS, &sd, 1);
	peer->SetMaximumIncomingConnections(MAX_CONNECTIONS - 1);

	registry.set<ConnectedClientMap>();
}


EntityAdmin::~EntityAdmin() {
	SLNet::RakPeerInterface::DestroyInstance(peer);
}

void EntityAdmin::update(float delta) {
	for (auto f : updateFunctions) {
		f(*this, delta);
	}
}

void EntityAdmin::addSystemUpdateFunction(EntityAdmin::function_ptr function) {
	updateFunctions.push_back(function);
}

SLNet::RakPeerInterface* EntityAdmin::getPeer() {
	return peer;
}