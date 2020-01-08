#include "EntityAdmin.h"
#include "RakPeerInterface.h"
#include "NetSystem.h"
#include "GameLogic.h"
#include "SingletonComponents.h"
#include "Components.h"
#include "Events.h"

#define MAX_CONNECTIONS 10
#define SERVER_PORT 60000

EntityAdmin::EntityAdmin() : peer(SLNet::RakPeerInterface::GetInstance()){
	addSystemUpdateFunction(System::net_update);
	addSystemUpdateFunction(GameLogic::input_system);
	addSystemUpdateFunction(GameLogic::afk_system);
	addSystemUpdateFunction(GameLogic::movement_system);
	addSystemUpdateFunction(System::net_disconnect);
	addSystemUpdateFunction(System::net_broadcast);

	registry.set<ConnectedClientMap>();
	registry.set<CloseConnectionBuffer>();

	registry.on_construct<NetClientComponent>().connect<&Event::net_client_connect>();
	registry.on_destroy<NetClientComponent>().connect<&Event::net_client_disconnect>();

	SLNet::SocketDescriptor sd(SERVER_PORT, nullptr);
	peer->Startup(MAX_CONNECTIONS, &sd, 1);
	peer->SetMaximumIncomingConnections(MAX_CONNECTIONS - 1);
	//peer->ApplyNetworkSimulator(0, 100, 0);
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