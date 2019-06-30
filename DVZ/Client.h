#pragma once

#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"

namespace Network {
	class Client
	{
	private:
		SLNet::RakPeerInterface *peer;

		SLNet::MessageID getPacketID(SLNet::Packet *packet);

	public:
		Client();
		~Client();

		void connect(const char *serverIP, unsigned short port);
		void poll();
	};
}