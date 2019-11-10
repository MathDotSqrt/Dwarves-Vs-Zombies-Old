#include "Block.h"
#include <limits.h>

using namespace Voxel;
#define IMG_WIDTH 256
#define IMG_HEIGHT 256

BlockManager::BlockManager(){
	BlockAttribs AIR(
		BlockType::BLOCK_TYPE_DEFAULT,
		MeshType::MESH_TYPE_NONE,
		OccludeType::OCCLUDE_TYPE_NONE,
		BlockTexCoords(),
		glm::u8vec4(0, 0, 0, 0)
	);
	addBlock(AIR);

	BlockAttribs STONE(
		BlockType::BLOCK_TYPE_STONE,
		MeshType::MESH_TYPE_BLOCK,
		OccludeType::OCCLUDE_TYPE_ALL,
		BlockTexCoords(0, 1, 16, 16),
		glm::u8vec4(77, 77, 77, 255)
	);
	addBlock(STONE);


	BlockAttribs DIRT(
		BlockType::BLOCK_TYPE_DIRT,
		MeshType::MESH_TYPE_BLOCK,
		OccludeType::OCCLUDE_TYPE_ALL,
		BlockTexCoords(2, 0, 16, 16),
		glm::u8vec4(128, 90, 70, 255)
	);
	addBlock(DIRT);

	BlockAttribs GRASS(
		BlockType::BLOCK_TYPE_GRASS,
		MeshType::MESH_TYPE_BLOCK,
		OccludeType::OCCLUDE_TYPE_ALL,
		BlockTexCoords(0, 0, 16, 16),
		glm::u8vec4(11, 102, 35, 255)
	);
	addBlock(GRASS);

	BlockAttribs SAND(
		BlockType::BLOCK_TYPE_SAND,
		MeshType::MESH_TYPE_BLOCK,
		OccludeType::OCCLUDE_TYPE_ALL,
		BlockTexCoords(14, 7, 16, 16),
		glm::u8vec4(194, 178, 128, 255)
	);
	addBlock(SAND);

	BlockAttribs PURPLE(
		BlockType::BLOCK_TYPE_PURPLE,
		MeshType::MESH_TYPE_BLOCK,
		OccludeType::OCCLUDE_TYPE_ALL,
		BlockTexCoords(1, 12, 16, 16),
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

BlockAttribs::BlockAttribs(BlockType block, MeshType mesh, OccludeType occlude, BlockTexCoords tex, glm::u8vec4 color) {
	blockType = block;
	meshType = mesh;
	occludeType = occlude;
	texcoords = tex;
	this->color = color;
}

BlockTexCoords::BlockTexCoords(){
	
}

BlockTexCoords::BlockTexCoords(uint8 r, uint8 c, uint8 numRows, uint8 numCols) {
	//constexpr UVType MAX = std::numeric_limits<UVType>::max();
	////constexpr UVType MAX = 1;
	//
	//const UVType SPRITE_WIDTH = MAX / numRows;
	//const UVType SPRITE_HEIGHT = MAX / numCols;	
	//
	//const UVType epsilon = 10;

	//UVType u = (UVType)(((float)r / numRows) * MAX);
	//UVType v = (UVType)(((float)c / numCols) * MAX);
	//UV uv(u+epsilon, v+epsilon);

	for (int i = 0; i < 6; i++) {
		this->texCoords[i].row = r;
		this->texCoords[i].col = c;
		
	}
}

SpriteTexCoords::SpriteTexCoords(UVType row, UVType col) {
	this->row = row;
	this->col = col;
}

UV SpriteTexCoords::uv0() {
	return UV(0, 0, row, col);
}

UV SpriteTexCoords::uv1() {
	//UV out = spritePos;
	//out.x += width;
	return UV(MAX, 0, row, col);
}

UV SpriteTexCoords::uv2() {
	//UV out = spritePos;
	//out.y += height;
	return UV(0, MAX, row, col);
}

UV SpriteTexCoords::uv3() {
	//UV out = spritePos;
	//out.x += width;
	//out.y += height;
	return UV(MAX, MAX, row, col);;
}

BlockManager* BlockManager::BLOCK_MANAGER_PTR = nullptr;