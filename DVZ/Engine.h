#pragma once
#include <set>
#include "entt.hpp"
#include "RakPeerInterface.h"
#include "System.h"	//todo remove this
#include "LinearAllocator.h"

#define MEM_ALLOC_SIZE 256 * 1024 * 1024

namespace Graphics {
	class OpenGLRenderer;
	class Scene;
	class ModelGeometry;
}

namespace Voxel {
	class ChunkManager;
}

class Engine : public entt::registry {
private:
	//MAIN PLAYER
	entt::entity main;
	//MAIN PLAYER

	//NET
	bool canSendPackets = false;
	const char* host;
	unsigned short port;
	SLNet::RakPeerInterface *peer;
	SLNet::SystemAddress *serverAddress;
	std::unordered_map<entt::entity, entt::entity> netToClientID;
	SLNet::MessageID getPacketID(SLNet::Packet *packet);
	//NET

	//SYSTEMS
	Util::Allocator::LinearAllocator linearAlloc;
	Voxel::ChunkManager *chunkManager;
	Graphics::OpenGLRenderer *renderer;
	Graphics::Scene *scene;
	std::set<System*, System::classcomp> systems;
	//SYSTEMS


public:
	Engine();
	~Engine();

	void update(float delta);

	entt::entity addPlayer(float x, float y, float z);
	entt::entity addNetPlayer(float x, float y, float z);
	entt::entity addBullet(entt::entity shooter);

	void attemptConnection(const char* host, unsigned short port);
	bool isConnected();

	void pollNetwork();
	void sendPacket();
	
	entt::entity getNetEntity(entt::entity netID);
	
	void addSystem(System *system);
	void removeSystem(System *system);
	void updateSystems(float delta);
	void deleteAllActiveSystems();

	entt::entity getPlayer();

	SLNet::ConnectionState getConnectionState();
	Util::Allocator::LinearAllocator& getAllocator();
	Voxel::ChunkManager* getChunkManager();
	Graphics::OpenGLRenderer* getRenderer();
	Graphics::Scene* getScene();
};

