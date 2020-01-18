#include "GameSystems.h"


#include "zlib.h"
#include "macrologger.h"
#include "Engine.h"
#include "Timer.h"
#include "Components.h"
#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"
#include "BitStream.h"
#include "GamePacketID.h"

#include "ChunkManager.h"

#include "Mesh.h"
#include "Scene.h"
#include <unordered_map>




void update_player(Engine &engine, SLNet::Packet *packet) {
	using namespace Component;
	
	SLNet::BitStream read(packet->data, packet->length, false);

	read.IgnoreBytes(sizeof(SLNet::MessageID));

	entt::entity playerID = entt::null;
	entt::entity netID = entt::null;
	glm::vec3 pos;
	glm::quat rot;

	read.Read(netID);
	read.Read(pos);
	read.Read(rot);

	auto &map = engine.ctx<std::unordered_map<entt::entity, entt::entity>>();

	auto iter = map.find(netID);
	if (iter == map.end()) {
		LOG_NET("NEW ENT");

		auto &scene = engine.ctx<Graphics::Scene>();
		auto &mesh_cache = engine.ctx<ResourceManager::MeshCache>();

		playerID = engine.create();
		engine.assign<Position>(playerID, pos);
		engine.assign<Rotation>(playerID, glm::quat(glm::vec3(0, 0, 0)));
		engine.assign<Scale>(playerID, glm::vec3(1, 1, 1));
		
		auto instanceID = scene.createRenderInstance(mesh_cache.handle("SpunkWalker"_hs), Graphics::NormalMaterial());
		engine.assign<RenderInstance>(playerID, instanceID);

		map[netID] = playerID;
	}
	else if(playerID = iter->second; playerID != engine.getPlayer()){
		auto &position = engine.get<Position>(playerID);
		auto &rotation = engine.get<Rotation>(playerID);
		position = pos;
		rotation = rot;
	}
}

void recieve_net_id(Engine &engine, SLNet::Packet *packet) {
	using namespace Component;
	SLNet::BitStream stream(packet->data, packet->length, false);

	auto &map = engine.ctx<std::unordered_map<entt::entity, entt::entity>>();

	stream.IgnoreBytes(sizeof(SLNet::MessageID));
	entt::entity playerID = engine.getPlayer();
	entt::entity netID = entt::null;
	stream.Read(netID);
	engine.assign_or_replace<Network>(playerID, netID);
	map.erase(netID);
	map[netID] = playerID;
	LOG_NET("CLIENT ID RECIEVED: %d", netID);
}

std::array<uint8, 10 * sizeof(uint8) * 1024> buffer;

void chunk_packet(Engine &engine, SLNet::Packet *packet) {
	auto &manager = engine.ctx<Voxel::ChunkManager>();

	int8 cx = 0;
	int8 cz = 0;
	uint32 inflate_bytes = 0;
	uint32 deflate_bytes = 0;

	SLNet::BitStream read(packet->data, packet->length, false);
	read.IgnoreBytes(sizeof(SLNet::MessageID));
	read.Read(cx);
	read.Read(cz);
	read.Read(inflate_bytes);
	read.Read(deflate_bytes);
	read.ReadAlignedBytes(buffer.data(), deflate_bytes);

	Voxel::RLEncoding encoding(inflate_bytes / sizeof(Voxel::RLElement));
	unsigned long source_length = inflate_bytes;
	uncompress((uint8*)encoding.data(), &source_length, buffer.data(), deflate_bytes);

	//todo buffer this at a later time...
	manager.loadChunkData(cx, 0, cz, encoding);
}

void block_delta(Engine &engine, SLNet::Packet *packet) {
	auto &manager = engine.ctx<Voxel::ChunkManager>();
	int8 cx = 0;
	int8 cz = 0;
	uint8 mod_diff = 0;

	SLNet::BitStream read(packet->data, packet->length, false);
	read.IgnoreBytes(sizeof(SLNet::MessageID));
	read.Read(cx);
	read.Read(cz);
	read.Read(mod_diff);

	auto chunk = manager.getChunk(cx, 0, cz);

	for (int i = 0; i < mod_diff; i++) {
		uint32 index = 0;
		Voxel::Block block;

		read.Read(index);
		read.Read(block);
		chunk->setBlock(index, block);
	}

}

SLNet::MessageID get_packet_id(SLNet::Packet *packet) {
	SLNet::MessageID id = (SLNet::MessageID)packet->data[0];

	if (id == ID_TIMESTAMP) {
		return (SLNet::MessageID)packet->data[sizeof(SLNet::MessageID) + sizeof(SLNet::Time)];
	}
	else {
		return (SLNet::MessageID)packet->data[0];
	}
}

void System::netword_system(Engine &engine, float delta) {
	Util::Performance::Timer netTimer("Packet parser");
	auto peer = engine.ctx<SLNet::RakPeerInterface*>();

	entt::entity id = engine.getPlayer();

	SLNet::Packet *packet;
	for (packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive()) {
		SLNet::MessageID id = get_packet_id(packet);


		switch (id) {
		case ID_CONNECTION_REQUEST_ACCEPTED:
			LOG_NET("CONNECTION ACCEPTED");
			break;
		case ID_PLAYER_MOVE:
			update_player(engine, packet);
			break;
		case ID_CLIENT_NET_ID:
			recieve_net_id(engine, packet);
			break;
		case ID_RL_CHUNK_DATA:
			chunk_packet(engine, packet);
			break;
		case ID_BLOCK_PLACE:
			block_delta(engine, packet);
			break;
		default:
			LOG_NET("ID CAUGHT: %d", id);
		}
	}
}

void System::send_packet_system(Engine &engine, float delta) {
	using namespace Component;
	
	auto view = engine.view<Player, Network>();
	if (view.size()) {
		entt::entity playerID = *view.begin();
		auto &pos = engine.get<Position>(playerID);
		auto &rot = engine.get<Rotation>(playerID);
		auto &input = engine.get<Input>(playerID);
		auto &net = engine.get<Network>(playerID);
		SLNet::BitStream out;
		out.Write((SLNet::MessageID)ID_CLIENT_INPUT);
		out.Write(rot);
		out.Write(input.up);
		out.Write(input.down);
		out.Write(input.left);
		out.Write(input.right);
		out.Write(input.space);
		out.Write(input.shift);
		out.Write(input.ctrl);
		//out.Write(input.mousePos[0]);
		//out.Write(input.mousePos[1]);


		auto peer = engine.ctx<SLNet::RakPeerInterface*>();
		peer->Send(&out, PacketPriority::LOW_PRIORITY, PacketReliability::UNRELIABLE, 0, SLNet::UNASSIGNED_RAKNET_GUID, true);

		auto &buffer = engine.getBlockPlaceBuffer();
		if(buffer.size()){
			SLNet::BitStream blockPlace;
			blockPlace.Write((SLNet::MessageID)ID_BLOCK_PLACE);

			while (buffer.size()) {
				auto element = buffer.back();
				blockPlace.Write(element.first);
				blockPlace.Write(element.second);				//saves three bytes due to alignment
				buffer.pop_back();
				break;											//todo allow for more blocks per packet
			}

			peer->Send(&blockPlace, PacketPriority::HIGH_PRIORITY, PacketReliability::UNRELIABLE, 0, SLNet::UNASSIGNED_RAKNET_GUID, true);
		}
		
	}
}