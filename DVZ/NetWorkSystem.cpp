#include "NetWorkSystem.h"
#include "Engine.h"
#include "Components.h"
#include "macrologger.h"

#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"
#include "BitStream.h"
#include "GamePacketID.h"

#include "QuadGeometry.h"
#include "Scene.h"

NetWorkSystem::NetWorkSystem(int priority, entt::entity clientEntt, Graphics::Scene *scene) : System(priority){
	this->clientEntt = clientEntt;
	this->scene = scene;
}

NetWorkSystem::~NetWorkSystem() {

}

void NetWorkSystem::addedToEngine(Engine * engine) {
	LOG_SYSTEM("Added to engine");

	this->peer = SLNet::RakPeerInterface::GetInstance();
	this->peer->Startup(1, &SLNet::SocketDescriptor(), 1);
	
	//"54.224.40.47"
	const char* host = "127.0.0.1";
	unsigned short port = 60000;
	this->peer->Connect(host, port, 0, 0);

	LOG_NET("Connecting to server [%s:%d]", host, port);

	engine->group<NetworkComponent>(entt::get<PositionComponent, RotationComponent>);
}

void NetWorkSystem::removedFromEngine(Engine * engine) {
	SLNet::RakPeerInterface::DestroyInstance(this->peer);
}

void NetWorkSystem::update(Engine * engine, float delta) {

	//SLNet::Packet *packet;

	//for (packet = this->peer->Receive(); packet; this->peer->DeallocatePacket(packet), packet = this->peer->Receive()) {
	//	SLNet::MessageID id = this->getPacketID(packet);
	//	LOG_NET("Packet received with ID [%d]", id);
	//	if (id == ID_CONNECTION_REQUEST_ACCEPTED) {
	//		LOG_NET("Connected");
	//		this->isConnected = true;

	//		SLNet::BitStream bs;
	//		bs.Write((SLNet::MessageID)Network::GamePacketID::ID_PLAYER_INFO);
	//		bs.Write(this->clientEntt);
	//		peer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
	//	}
	//	else if (id == Network::GamePacketID::ID_NEW_PLAYER_CONNECTED) {
	//		SLNet::BitStream in(packet->data, packet->length, false);
	//		in.IgnoreBytes(sizeof(SLNet::MessageID));
	//		unsigned int ID;
	//		in.Read(ID);
	//		
	//		LOG_NET("Other player connected with ID: %d", ID);

	//		//Graphics::QuadGeometry quad;
	//		//unsigned int meshID = scene->createBasicMesh(quad, 1, 0, 0);
	//		//unsigned int instanceID = scene->createInstance(meshID, glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(.1, .1, .1));

	//		//entt::entity newEntt = engine.create();
	//		//engine.assign<PositionComponent>(newEntt, glm::vec3(0, 0, -1));
	//		//engine.assign<RotationComponent>(newEntt, glm::quat(1, 0, 0, 0));
	//		//engine.assign<ScaleComponent>(newEntt, glm::vec3(.1, .1, .1));
	//		//engine.assign<RenderInstanceComponent>(newEntt, instanceID);
	//		//engine.assign<NetworkComponent>(newEntt, instanceID);

	//		//this->netToEnttID[ID] = newEntt;
	//	}
	//	else if (id == Network::GamePacketID::ID_ENTITY_POS) {
	//		LOG_NET("OTHER PLAYER MOVED");
	//	}


	//}
}

SLNet::MessageID NetWorkSystem::getPacketID(SLNet::Packet *packet) {
	if (packet->data[0] == ID_TIMESTAMP) {
		return packet->data[sizeof(SLNet::MessageID) + sizeof(SLNet::Time)];
	}
	else {
		return packet->data[0];
	}
}