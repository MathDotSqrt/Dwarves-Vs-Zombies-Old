#include "VoxelUtil.h"
#include "Components.h"

using namespace Voxel;

Voxel::ChunkModStamp& index_snapshot(int cx, int cz, ClientChunkSnapshotComponent &snap) {
	const auto WIDTH = snap.VIEW_DIST;
	const auto RADIUS = snap.VIEW_RADIUS;
	
	int r = (cz + RADIUS) % WIDTH;
	int c = (cx + RADIUS) % WIDTH;
	int new_r = r >= 0 ? r : r + WIDTH;
	int new_c = c >= 0 ? c : c + WIDTH;

	return snap.snapshot[r][c];
}

ChunkModStamp& Voxel::getChunkModCounter(
	ChunkBoundryComponent &boundry, 
	ClientChunkSnapshotComponent &snapshot) {
	
	return index_snapshot(boundry.x, boundry.z, snapshot);
}

ChunkModStamp& Voxel::getChunkModCounter(
	glm::i32vec3 coord,
	ClientChunkSnapshotComponent &snapshot) {

	return index_snapshot(coord.x, coord.z, snapshot);
}

void Voxel::setSnapshotCenter(
	glm::i32vec3 center_chunk,
	ClientChunkSnapshotComponent &snapshot) {

	const auto RADIUS = ClientChunkSnapshotComponent::VIEW_RADIUS;
	for (int z = -RADIUS; z <= RADIUS; z++) {
		for (int x = -RADIUS; x <= RADIUS; x++) {
			const glm::i32vec3 chunk_coord = center_chunk + glm::i32vec3(x, 0, z);
			auto &mod_counter = index_snapshot(chunk_coord.x, chunk_coord.z, snapshot);

			//if mod_counter coord does not match chunk_coord; 
			//new chunks so set zero mod count for chunk snapshot
			if (mod_counter.getCX() != chunk_coord.x
				//|| mod_counter.getCY() != chunk_coord.y
				|| mod_counter.getCZ() != chunk_coord.z) {
				mod_counter = ChunkModStamp(chunk_coord.x, 0, chunk_coord.z);
			}

		}
	}


}
