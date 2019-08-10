#pragma once
#include <set>
#include "entt.hpp"
#include "RakPeerInterface.h"
#include "System.h"
#include "ModelGeometry.h"

namespace Graphics {
	class OpenGLRenderer;
	class Scene;
}

namespace Voxel {
	class ChunkManager;
}

class Engine : public entt::registry{
private:
	entt::entity main;
	bool canSendPackets = false;

	const char* host;
	unsigned short port;
	SLNet::RakPeerInterface *peer;
	SLNet::SystemAddress *serverAddress;

	std::unordered_map<entt::entity, entt::entity> netToClientID;

	Voxel::ChunkManager *chunkManager;
	Graphics::OpenGLRenderer *renderer;
	Graphics::Scene *scene;
	std::set<System*, System::classcomp> systems;

	Graphics::ModelGeometry *model;

	SLNet::MessageID getPacketID(SLNet::Packet *packet);

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


	SLNet::ConnectionState getConnectionState();
	Voxel::ChunkManager* getChunkManager();
	Graphics::OpenGLRenderer* getRenderer();
	Graphics::Scene* getScene();
};

