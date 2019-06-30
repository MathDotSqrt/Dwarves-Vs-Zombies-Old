#include "Client.h"
#include "macrologger.h"

using namespace Network;
Client::Client() {
	this->peer = SLNet::RakPeerInterface::GetInstance();
}


Client::~Client() {

	SLNet::RakPeerInterface::DestroyInstance(this->peer);
}

void Client::connect(const char *serverIP, unsigned short port) {
	this->peer->Startup(1, &SLNet::SocketDescriptor(), 1);
	this->peer->Connect(serverIP, port, 0, 0);

	LOG_NET("Connecting to server [%s:%d]", serverIP, port);
}

void Client::poll() {
	SLNet::Packet *packet;

	for (packet = this->peer->Receive(); packet; this->peer->DeallocatePacket(packet), packet = this->peer->Receive()) {
		SLNet::MessageID id = this->getPacketID(packet);
		LOG_NET("Packet received with ID [%d]", id);
		if (id == ID_CONNECTION_REQUEST_ACCEPTED) {
			LOG_NET("Connected");
		}
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