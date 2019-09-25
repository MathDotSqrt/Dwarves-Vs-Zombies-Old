#include "Engine.h"

#include <thread>
#include <list>
#include <algorithm>

#include "GamePacketID.h"
#include "BitStream.h"

#include "Components.h"
#include "System.h"
#include "Scene.h"
#include "OpenGLRenderer.h"
#include "QuadGeometry.h"
#include "ModelGeometry.h"
#include "ChunkManager.h"
#include "Timer.h"

using namespace std;

Engine::Engine() {
	this->peer = SLNet::RakPeerInterface::GetInstance();
	this->serverAddress = nullptr;

	this->scene = new Graphics::Scene();
	this->renderer = new Graphics::OpenGLRenderer();
	this->renderer->init(scene);

	this->chunkManager = new Voxel::ChunkManager();

	this->main = entt::null;
}


Engine::~Engine(){
	delete this->serverAddress;
	delete this->scene;
	delete this->renderer;
	delete this->chunkManager;

	SLNet::RakPeerInterface::DestroyInstance(this->peer);
	this->deleteAllActiveSystems();
}

entt::entity Engine::addPlayer(float x, float y, float z) {
	if (this->main != entt::null) {
		LOG_WARNING("Attempting to make two main entities...");
		return this->main;
	}
	
	entt::entity id = this->create();
	this->assign<PositionComponent>(id, glm::vec3(x, y, z));
	this->assign<RotationComponent>(id, glm::quat(1, 0, 0, 0));
	this->assign<VelocityComponent>(id, glm::vec3(0, 0, 0));
	this->assign<RotationalVelocityComponent>(id, glm::vec3(0, 0, 0));
	this->assign<DirComponent>(id, glm::vec3(0, 0, -1), glm::vec3(0, 1, 0), glm::vec3(1, 0, 0));
	this->assign<InputComponent>(id);
	this->assign<CameraInstanceComponent>(id, this->scene->getMainCameraID());
	this->assign<NetworkComponent>(id);

	this->main = id;

	return id;
}



entt::entity Engine::addNetPlayer(float x, float y, float z) {
	entt::entity id = this->create();

	//this->assign<PositionComponent>(id, glm::vec3(x, y, z));
	//this->assign<RotationComponent>(id, glm::quat(1, 0, 0, 0));
	//this->assign<ScaleComponent>(id, glm::vec3(.3, .3, .3));
	//this->assign<VelocityComponent>(id, glm::vec3(0, 0, 0));
	//this->assign<RotationalVelocityComponent>(id, glm::vec3(0, 0, 0));
	//this->assign<NetworkComponent>(id);

	//Graphics::ColorMaterial c = {1, 0, 1};
	//unsigned int meshID = this->scene->createMesh(*this->model, c);
	//unsigned int renderID = this->scene->createRenderInstance(meshID);
	//this->assign<RenderInstanceComponent>(id, renderID);

	return id;
}

void Engine::update(float delta) {
	//this->pollNetwork();
	{
		Util::Performance::Timer timer("Update Systems");
		this->updateSystems(delta);
	}

	this->renderer->prerender();
	this->renderer->render(this->chunkManager);
	
	/*VelocityComponent &v = this->get<VelocityComponent>(this->main);
	RotationalVelocityComponent &rv = this->get<RotationalVelocityComponent>(this->main);
	float velLen = glm::length2(v.vel);
	float rVelLen = glm::length2(rv.eular);
	bool isMoving = (velLen > .01f || rVelLen > .01f);

	if (this->canSendPackets && isMoving && this->isConnected()) {
		NetworkComponent &n = this->get<NetworkComponent>(this->main);
		PositionComponent &p = this->get<PositionComponent>(this->main);
		RotationComponent &r = this->get<RotationComponent>(this->main);
		glm::vec3 eular = glm::eulerAngles(r.rot);

		SLNet::BitStream out;
		out.Write((SLNet::MessageID)Network::GamePacketID::ID_PLAYER_POS);
		out.Write(n.netID);
		out.Write(p.pos.x);
		out.Write(p.pos.y - 1);
		out.Write(p.pos.z);
		out.Write(eular.x);
		out.Write(eular.y);
		out.Write(eular.z);

		this->peer->Send(&out, HIGH_PRIORITY, RELIABLE_ORDERED, 0, SLNet::UNASSIGNED_RAKNET_GUID, true);
	}*/
}

void Engine::attemptConnection(const char* host, unsigned short port) {
	this->host = host;
	this->port = port;
	this->serverAddress = new SLNet::SystemAddress(host, port);

	SLNet::SocketDescriptor sd;

	this->peer->Startup(1, &sd, 1);
	this->peer->Connect(this->host, this->port, 0, 0);

	LOG_ENGINE("Connection attempted");
}

void Engine::pollNetwork() {
	SLNet::Packet *packet;
	for (packet = this->peer->Receive(); packet; this->peer->DeallocatePacket(packet), packet = this->peer->Receive()) {
		SLNet::MessageID id = this->getPacketID(packet);
		//LOG_NET("Packet Recieved with id [%d]", id);
		switch (id) {
		case ID_CONNECTION_REQUEST_ACCEPTED:
			LOG_NET("Connected to %s|%d", this->host, this->port);
			break;
		case Network::GamePacketID::ID_CLIENT_NET_ID:
		{
			SLNet::BitStream in(packet->data, packet->length, false);
			in.IgnoreBytes(sizeof(SLNet::MessageID));
			entt::entity netID;
			in.Read(netID);

			NetworkComponent &n = this->get<NetworkComponent>(this->main);
			n.netID = netID;
			n.delta = 0;

			this->canSendPackets = true;

			LOG_NET("Recieved Client Net ID [%d]", netID);
			break;
		}
		case Network::GamePacketID::ID_PLAYER_POS:
			SLNet::BitStream in(packet->data, packet->length, false);
			in.IgnoreBytes(sizeof(SLNet::MessageID));

			entt::entity netID;
			in.Read(netID);
			entt::entity clientID = this->getNetEntity(netID);

			PositionComponent &p = this->get<PositionComponent>(clientID);
			RotationComponent &r = this->get<RotationComponent>(clientID);

			struct {
				float x, y, z;
			} pos, rot;

			in.Read(pos);
			in.Read(rot);

			p.pos = glm::vec3(pos.x, pos.y, pos.z);
			r.rot = glm::quat(glm::vec3(rot.x, rot.y + 3.1415926, rot.z));
		}
	}
}

entt::entity Engine::getNetEntity(entt::entity netID) {
	entt::entity clientID;
	if (this->netToClientID.find(netID) == this->netToClientID.end()) {
		clientID = this->addNetPlayer(0, 0, 0);
		NetworkComponent &n = this->get<NetworkComponent>(clientID);
		n.netID = clientID;
		n.delta = 0;

		netToClientID[netID] = clientID;
		LOG_NET("NEW PLAYER CONNECTED NET ID[%d] CLIENT ID[%d]", netID, clientID);
	}
	else {
		clientID = this->netToClientID[netID];
	}

	return clientID;

}

bool Engine::isConnected() {
	return this->getConnectionState() == SLNet::ConnectionState::IS_CONNECTED;
}

void Engine::addSystem(System *system) {
	if (std::find(this->systems.begin(), this->systems.end(), system) == this->systems.end()) {
		//LOG_ENGINE("System: %d Index: %d", system->getPriority(), std::upper_bound(this->systems.begin(), this->systems.end(), system, system->operator<));
		this->systems.insert(system);	//TODO fix bug where multiple systems cannot have same priority
		
		//this->systems.push_back(system);
		system->addedToEngine(this);
	}
}

void Engine::removeSystem(System *system) {
	if (find(this->systems.begin(), this->systems.end(), system) != this->systems.end()) {
		system->removedFromEngine(this);
		//this->systems.erase(std::remove(systems.begin(), systems.end(), system));
		this->systems.erase(find(this->systems.begin(), this->systems.end(), system));
	}
}

void Engine::updateSystems(float delta) {
	for (auto *system : this->systems) {
		system->update(this, delta);
	}
}

void Engine::deleteAllActiveSystems() {
	for (System *system : this->systems) {
		system->removedFromEngine(this);
		delete system;
	}

	this->systems.clear();
}

entt::entity Engine::getPlayer() {
	return this->main;
}

SLNet::ConnectionState Engine::getConnectionState() {
	const double delta = 100;	//Only poll every 10th of a second
	static SLNet::ConnectionState lastState = SLNet::IS_NOT_CONNECTED;
	static clock_t lastTime = 0L;

	if (this->serverAddress == nullptr)
		return SLNet::IS_NOT_CONNECTED;


	if (clock() - lastTime >= delta) {
		lastState = this->peer->GetConnectionState(*this->serverAddress);
		lastTime = clock();
	}
	
	return lastState;
}

SLNet::MessageID Engine::getPacketID(SLNet::Packet *packet) {
	if (packet->data[0] == ID_TIMESTAMP) {
		return packet->data[sizeof(SLNet::MessageID) + sizeof(SLNet::Time)];
	}
	else {
		return packet->data[0];
	}
}

Voxel::ChunkManager* Engine::getChunkManager() {
	return this->chunkManager;
}

Graphics::OpenGLRenderer* Engine::getRenderer() {
	return this->renderer;
}

Graphics::Scene* Engine::getScene() {
	return this->scene;
}
