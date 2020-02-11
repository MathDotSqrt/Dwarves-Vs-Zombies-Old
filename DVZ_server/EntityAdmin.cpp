#include "EntityAdmin.h"
#include "RakPeerInterface.h"
#include "NetSystem.h"
#include "GameLogic.h"
#include "SingletonComponents.h"
#include "Components.h"
#include "Events.h"
#include "ChunkManager.h"

#define MAX_CONNECTIONS 10
#define SERVER_PORT 60000
#define ALLOC_SIZE 500 * 1024 * 1024

EntityAdmin::EntityAdmin() : peer(SLNet::RakPeerInterface::GetInstance()), allocator(ALLOC_SIZE, malloc(ALLOC_SIZE)){
	addSystemUpdateFunction(StatelessSystem(System::net_update));
	addSystemUpdateFunction(StatelessSystem(GameLogic::gravity_system));
	addSystemUpdateFunction(StatelessSystem(GameLogic::input_system));
	addSystemUpdateFunction(StatelessSystem(GameLogic::afk_system));
	addSystemUpdateFunction(StatelessSystem(GameLogic::voxel_collision_system));
	addSystemUpdateFunction(StatelessSystem(GameLogic::movement_system));
	addSystemUpdateFunction(StatelessSystem(GameLogic::voxel_system));
	addSystemUpdateFunction(StatelessSystem(System::net_disconnect));
	addSystemUpdateFunction(StatelessSystem(System::net_broadcast));
	addSystemUpdateFunction(StatelessSystem(System::net_voxel, StatelessSystem::FrameTime(.2)));

	registry.set<ConnectedClientMap>();
	registry.set<CloseConnectionBuffer>();
	printf("Generating Terrain...\n");
	registry.set<Voxel::ChunkManager>(allocator);
	printf("Done.\n");

	registry.on_construct<Component::NetClient>().connect<&Event::net_client_connect>();
	registry.on_destroy<Component::NetClient>().connect<&Event::net_client_disconnect>();

	SLNet::SocketDescriptor sd(SERVER_PORT, nullptr);
	peer->Startup(MAX_CONNECTIONS, &sd, 1);
	peer->SetMaximumIncomingConnections(MAX_CONNECTIONS - 1);
	//peer->ApplyNetworkSimulator(0, 5000, 0);
}


EntityAdmin::~EntityAdmin() {
	SLNet::RakPeerInterface::DestroyInstance(peer);
}

void EntityAdmin::update(float delta) {
	for (auto &system : updateFunctions) {
		system.update(*this, delta);
	}
}

void EntityAdmin::addSystemUpdateFunction(StatelessSystem system) {
	updateFunctions.push_back(system);
}

SLNet::RakPeerInterface* EntityAdmin::getPeer() {
	return peer;
}

Voxel::ChunkManager& EntityAdmin::getChunkManager() {
	return registry.ctx<Voxel::ChunkManager>();
}