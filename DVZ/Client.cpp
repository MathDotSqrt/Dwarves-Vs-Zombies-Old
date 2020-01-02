#include "Client.h"
#include "macrologger.h"
#include "BitStream.h"
#include "GamePacketID.h"

using namespace Network;

Client::Client() {
	this->peer = SLNet::RakPeerInterface::GetInstance();
}


Client::~Client() {
	SLNet::RakPeerInterface::DestroyInstance(this->peer);
	delete this->systemAddress;
}

void Client::sendConnectionRequest(const char *serverIP, unsigned short port) {
	delete this->systemAddress;
	this->systemAddress = new SLNet::SystemAddress(serverIP, port);

	this->peer->Startup(1, &SLNet::SocketDescriptor(), 1);
	this->peer->Connect(serverIP, port, 0, 0);

	LOG_NET("Requesting connection to [%s:%d]", serverIP, port);
}

//Dont call this function too much, causes mutex lock and may stall the connection process
bool Client::isConnected() {
	return this->peer->GetConnectionState(*this->systemAddress) == SLNet::ConnectionState::IS_CONNECTED;
}

void Client::poll() {
	SLNet::Packet *packet;
	for (packet = this->peer->Receive(); packet; this->peer->DeallocatePacket(packet), packet = this->peer->Receive()) {
		SLNet::MessageID id = this->getPacketID(packet);
		LOG_NET("Packet received with ID [%d]", id);
		if (id == ID_CONNECTION_REQUEST_ACCEPTED) {
			LOG_NET("Connected accepted");
		}
		/*else if (id == GamePacketID::ID_SPAWN_PLAYER) { 
			LOG_NET("Spawn player");
		}*/
		//else if (id == GamePacketID::ID_PLAYER_POS) {
		//	LOG_NET("Player move");
		//}
	}

}

SLNet::MessageID Client::getPacketID(SLNet::Packet *packet) {
	if (packet->data[0] == ID_TIMESTAMP) {
		return packet->data[sizeof(SLNet::MessageID) + sizeof(SLNet::Time)];
	}
	else {
		return packet->data[0];
	}
}

void Client::send(const char* str) {
	//LOG_NET("Set msg: %s", str);

	SLNet::BitStream bs;
	//bs.Write((SLNet::MessageID)GamePacketID::ID_GAME_MESSAGE);
	bs.Write(str);

	peer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, SLNet::UNASSIGNED_RAKNET_GUID, true);
}

void Client::send(float x, float y, float z) {
	struct {
		float x, y, z;
	} vec;
	vec.x = x;
	vec.y = y;
	vec.z = z;

	SLNet::BitStream bs;
	//bs.Write((SLNet::MessageID)GamePacketID::ID_ENTITY_POS);
	bs.Write(vec);

	
	peer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, SLNet::UNASSIGNED_RAKNET_GUID, true);
}

