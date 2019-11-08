#include "Block.h"

using namespace Voxel;

BlockManager::BlockManager(){
	BlockAttribs AIR(
		BlockType::BLOCK_TYPE_DEFAULT,
		MeshType::MESH_TYPE_NONE,
		OccludeType::OCCLUDE_TYPE_NONE,
		glm::u8vec4(0, 0, 0, 0)
	);
	addBlock(AIR);

	BlockAttribs STONE(
		BlockType::BLOCK_TYPE_STONE,
		MeshType::MESH_TYPE_BLOCK,
		OccludeType::OCCLUDE_TYPE_ALL,
		glm::u8vec4(77, 77, 77, 255)
	);
	addBlock(STONE);


	BlockAttribs DIRT(
		BlockType::BLOCK_TYPE_DIRT,
		MeshType::MESH_TYPE_BLOCK,
		OccludeType::OCCLUDE_TYPE_ALL,
		glm::u8vec4(179, 102, 102, 255)
	);
	addBlock(DIRT);

	BlockAttribs GRASS(
		BlockType::BLOCK_TYPE_GRASS,
		MeshType::MESH_TYPE_BLOCK,
		OccludeType::OCCLUDE_TYPE_ALL,
		glm::u8vec4(0, 255, 0, 255)
	);
	addBlock(GRASS);

	BlockAttribs PURPLE(
		BlockType::BLOCK_TYPE_PURPLE,
		MeshType::MESH_TYPE_BLOCK,
		OccludeType::OCCLUDE_TYPE_ALL,
		glm::u8vec4(179, 0, 255, 255)
	);
	addBlock(PURPLE);
}

void BlockManager::addBlock(BlockAttribs block) {
	this->blockAttribs[block.blockType] = block;
}

BlockAttribs::BlockAttribs(){
	blockType = BlockType::BLOCK_TYPE_INVALID;
	meshType = MeshType::MESH_TYPE_BLOCK;
	occludeType = OccludeType::OCCLUDE_TYPE_ALL;
	color = glm::u8vec4(255, 0, 255, 255);
}

BlockAttribs::BlockAttribs(BlockType block, MeshType mesh, OccludeType occlude, glm::u8vec4 color) {
	blockType = block;
	meshType = mesh;
	occludeType = occlude;
	this->color = color;
}

BlockTexCoords::BlockTexCoords(){
	
}

BlockTexCoords::BlockTexCoords(int r, int c, int numRows, int numCols) {

}

BlockManager* BlockManager::BLOCK_MANAGER_PTR = nullptr;