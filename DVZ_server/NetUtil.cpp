#include "NetUtil.h"
#include "SingletonComponents.h"

SLNet::MessageID getPacketID(SLNet::Packet *packet) {
	SLNet::MessageID id = (SLNet::MessageID)packet->data[0];

	if (id == ID_TIMESTAMP) {
		return (SLNet::MessageID)packet->data[sizeof(SLNet::MessageID) + sizeof(SLNet::Time)];
	}
	else {
		return (SLNet::MessageID)packet->data[0];
	}
}

void closeClientConnection(EntityAdmin &admin, SLNet::RakNetGUID guid) {
	auto &buffer = admin.registry.ctx<CloseConnectionBuffer>();
	buffer.push_back(guid);
}