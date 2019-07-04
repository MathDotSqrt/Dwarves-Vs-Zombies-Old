#pragma once

#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"

namespace Network {
	class Client
	{
	private:
		SLNet::RakPeerInterface *peer;
		SLNet::MessageID getPacketID(SLNet::Packet *packet);
		SLNet::SystemAddress *systemAddress;
	public:
		Client();
		~Client();

		void sendConnectionRequest(const char *serverIP, unsigned short port);
		bool isConnected();
		
		void poll();
		

		void send(const char* str);
		void send(float x, float y, float z);
	};
}