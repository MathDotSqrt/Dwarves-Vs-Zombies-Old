#include "EntityAdmin.h"
#include "Components.h"
#include "SingletonComponents.h"


#include "NetSystem.h"
#include "MessageIdentifiers.h"
#include "RakNetTypes.h"
#include "GamePacketID.h"
#include "BitStream.h"
#include "NetUtil.h"

#include "VoxelUtil.h"
#include "ChunkManager.h"
#include "RLEncoder.h"

using namespace System;
using namespace SLNet;

void incomming_connection_packet(RakPeerInterface *peer, Packet *packet, entt::registry &registry) {
	printf("New connection from [%s]\n", packet->systemAddress.ToString());

	entt::entity player = registry.create();
	registry.assign<NetClientComponent>(player, packet->guid);
	registry.assign<PositionComponent>(player, glm::vec3(0, 0, 0));
	registry.assign<RotationComponent>(player, glm::vec3(0, 0, 0));
	registry.assign<VelocityComponent>(player, glm::vec3(0, 0, 0));
	registry.assign<DirComponent>(player, glm::vec3(0, 0, -1), glm::vec3(0, 1, 0), glm::vec3(1, 0, 0));
	registry.assign<InputComponent>(player);
	registry.assign<AFKComponent>(player);
	registry.assign<ChunkBoundryComponent>(player, glm::vec3(0, 0, 0));
	registry.assign<ClientChunkSnapshotComponent>(player);
	
	BitStream out;
	out.Write((MessageID)ID_CLIENT_NET_ID);
	out.Write(player);
	peer->Send(&out, PacketPriority::IMMEDIATE_PRIORITY, PacketReliability::RELIABLE, 0, packet->systemAddress, false);
}

void disconnection_packet(Packet *packet, entt::registry &registry) {
	printf("CLIENT DISCONNECTED\n");
	auto &map = registry.ctx<ConnectedClientMap>();
	
	if (auto iter = map.find(packet->guid); iter != map.end()) {
		entt::entity playerID = iter->second;
		registry.destroy(playerID);		//destroy should call a signal handler to manager ConnectedClientMap
	}
}

void client_input_packet(Packet *packet, entt::registry &registry) {
	auto &map = registry.ctx<ConnectedClientMap>();
	auto iter = map.find(packet->guid);

	if (iter == map.end()) {
		return;
	}

	entt::entity clientEntity = map[packet->guid];

	//auto &pos = registry.get<PositionComponent>(clientEntity);
	auto &rot = registry.get<RotationComponent>(clientEntity);
	auto &input = registry.get<InputComponent>(clientEntity);

	BitStream in(packet->data, packet->length, false);
	in.IgnoreBytes(sizeof(MessageID));
	//in.Read(pos);
	in.Read(rot);
	input.up = in.ReadBit();
	input.down = in.ReadBit();
	input.left = in.ReadBit();
	input.right = in.ReadBit();
	input.space = in.ReadBit();
	input.shift = in.ReadBit();
	input.ctrl = in.ReadBit();
}

void block_place_packet(Packet *packet, EntityAdmin &admin) {
	
	auto peer = admin.getPeer();

	BitStream in(packet->data, packet->length, false);
	in.IgnoreBytes(sizeof(MessageID));
	
	
	glm::i32vec3 blockPos;
	Voxel::Block block;
	in.Read(blockPos);
	in.Read(block);

	auto &manager = admin.getChunkManager();
	manager.setBlock(blockPos.x, blockPos.y, blockPos.z, block);

	admin.registry.view<ClientChunkSnapshotComponent>().each([](auto &snap) {
		snap.has_origin = false;
	});

}

void System::net_update(EntityAdmin &admin, float delta) {
	entt::registry &registry = admin.registry;
	auto peer = admin.getPeer();

	Packet *packet = nullptr;
	for (packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive()) {
		MessageID id = getPacketID(packet);

		switch (id) {
		case ID_NEW_INCOMING_CONNECTION: 
			incomming_connection_packet(peer, packet, registry);
			break;
		case ID_DISCONNECTION_NOTIFICATION: 
			disconnection_packet(packet, registry);
			break;
		case ID_CLIENT_INPUT:
			client_input_packet(packet, registry);
			break;
		case ID_BLOCK_PLACE:
			block_place_packet(packet, admin);
			break;
		default:
			printf("Uncaught packet [%d]\n", id);
			break;
		}
	}
}

void System::net_disconnect(EntityAdmin &admin, float delta){
	auto &registry = admin.registry;
	auto peer = admin.getPeer();
	auto &buffer = registry.ctx<CloseConnectionBuffer>();

	if (buffer.size()) {
		auto guid = buffer.back();
		peer->CloseConnection(guid, true);
		buffer.pop_back();
	}
}

void System::net_broadcast(EntityAdmin &admin, float delta) {
	auto &registry = admin.registry;
	auto *peer = admin.getPeer();
	auto &map = registry.ctx<ConnectedClientMap>();

	auto view = registry.view<PositionComponent, RotationComponent>();

	for (entt::entity e : view) {
		PositionComponent pos = view.get<PositionComponent>(e);
		RotationComponent rot = view.get<RotationComponent>(e);

		BitStream stream;
		stream.Write((MessageID)ID_PLAYER_MOVE);
		stream.Write(e);
		stream.Write(pos);
		stream.Write(rot);
		peer->Send(&stream, PacketPriority::MEDIUM_PRIORITY, PacketReliability::UNRELIABLE, 0, UNASSIGNED_RAKNET_GUID, true);
	}
}

void System::net_voxel(EntityAdmin &admin, float delta) {
	auto &registry = admin.registry;
	auto &manager = admin.getChunkManager();
	auto *peer = admin.getPeer();
	auto view = registry.view<NetClientComponent, ChunkBoundryComponent, ClientChunkSnapshotComponent>();
	view.each([&manager, peer](auto &net, auto &bound, auto &snapshot) {
		
		//todo only call this when player moves boundries
		Voxel::setSnapshotCenter(bound, snapshot);

		auto RADIUS = ClientChunkSnapshotComponent::VIEW_RADIUS;
		for (int z = -RADIUS; z <= RADIUS; z++) {
			for (int x = -RADIUS; x <= RADIUS; x++) {
				const auto chunk_pos = bound + glm::i32vec3(x, 0, z);
				auto &stamp = Voxel::getChunkModCounter(chunk_pos, snapshot);

				if (stamp.getModificationCount() == 0) {
					const auto &chunk = manager.getChunk(chunk_pos.x, chunk_pos.z);
					const auto rl_chunk = Voxel::encode_chunk(chunk);
					const unsigned char * ptr = (unsigned char*)rl_chunk.data();
					const unsigned int num_bytes = (unsigned int)(sizeof(rl_chunk[0]) * rl_chunk.size());
					
					printf("Chunk packet size: [%d]\n", num_bytes);

					
					SLNet::RakNetGUID guid = net.guid;
					BitStream stream;
					stream.Write((MessageID)ID_RL_CHUNK_DATA);
					stream.Write((uint8)chunk_pos.x);
					stream.Write((uint8)chunk_pos.z);
					stream.Write(num_bytes);
					stream.WriteAlignedBytes(ptr, num_bytes);
					//peer->Send(&stream, PacketPriority::LOW_PRIORITY, PacketReliability::RELIABLE_WITH_ACK_RECEIPT, 0, guid, false);
					peer->Send(&stream, PacketPriority::LOW_PRIORITY, PacketReliability::RELIABLE, 0, guid, false);
					
					stamp.increment();
				}
			}
		}
		
	});
}