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
		LightType::LIGHT_TYPE_TRANSPARENT,
		BlockTexCoords(),
		glm::u8vec4(0, 0, 0, 0)
	);
	addBlock(AIR);

	BlockAttribs STONE(
		BlockType::BLOCK_TYPE_STONE,
		MeshType::MESH_TYPE_BLOCK,
		OccludeType::OCCLUDE_TYPE_ALL,
		//BlockTexCoords(1, 0, 2, 2),
		BlockTexCoords(0, 1, 16, 16),
		glm::u8vec4(77, 77, 77, 255)
	);
	addBlock(STONE);


	BlockAttribs DIRT(
		BlockType::BLOCK_TYPE_DIRT,
		MeshType::MESH_TYPE_BLOCK,
		OccludeType::OCCLUDE_TYPE_ALL,
		//BlockTexCoords(0, 0, 2, 2),
		BlockTexCoords(2, 0, 16, 16),
		glm::u8vec4(128, 90, 70, 255)
	);
	addBlock(DIRT);

	BlockAttribs GRASS(
		BlockType::BLOCK_TYPE_GRASS,
		MeshType::MESH_TYPE_BLOCK,
		OccludeType::OCCLUDE_TYPE_ALL,
		//BlockTexCoords({ 0, 1, 0, 1 }, 2, 2),
		BlockTexCoords({ 0, 0, 3, 0 }, 16, 16),
		glm::u8vec4(11, 102, 35, 255)
	);
	addBlock(GRASS);

	BlockAttribs SAND(
		BlockType::BLOCK_TYPE_SAND,
		MeshType::MESH_TYPE_BLOCK,
		OccludeType::OCCLUDE_TYPE_ALL,
		//BlockTexCoords(0, 0, 2, 2),
		BlockTexCoords(14, 7, 16, 16),
		glm::u8vec4(194, 178, 128, 255)
	);
	addBlock(SAND);

	BlockAttribs GLASS(
		BlockType::BLOCK_TYPE_GLASS,
		MeshType::MESH_TYPE_BLOCK,
		OccludeType::OCCLUDE_TYPE_SELF,
		LightType::LIGHT_TYPE_TRANSPARENT,
		BlockTexCoords(1, 3, 16, 16),
		glm::u8vec4(194, 178, 128, 255)
	);
	addBlock(GLASS);

	BlockAttribs ROSE(
		BlockType::BLOCK_TYPE_ROSE,
		MeshType::MESH_TYPE_X,
		OccludeType::OCCLUDE_TYPE_NONE,
		LightType::LIGHT_TYPE_TRANSPARENT,
		BlockTexCoords(12, 0, 16, 16),
		glm::u8vec4(194, 178, 128, 255)
	);
	addBlock(ROSE);

	BlockAttribs PURPLE(
		BlockType::BLOCK_TYPE_PURPLE,
		MeshType::MESH_TYPE_BLOCK,
		OccludeType::OCCLUDE_TYPE_ALL,
		LightType::LIGHT_TYPE_SOURCE,
		BlockTexCoords(1, 12, 16, 16),
		glm::u8vec4(179, 0, 255, 255)
	);
	addBlock(PURPLE);

	BlockAttribs INVALID(
		BlockType::BLOCK_TYPE_INVALID,
		MeshType::MESH_TYPE_BLOCK,
		OccludeType::OCCLUDE_TYPE_ALL,
		LightType::LIGHT_TYPE_SOURCE,
		BlockTexCoords(1, 12, 16, 16),
		glm::u8vec4(179, 0, 255, 255)
	);
	addBlock(INVALID);
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
	lightType = LightType::LIGHT_TYPE_OPAQUE;
	texcoords = tex;
	this->color = color;
}

BlockAttribs::BlockAttribs(BlockType block, MeshType mesh, OccludeType occlude, LightType light, BlockTexCoords tex, glm::u8vec4 color) {
	blockType = block;
	meshType = mesh;
	occludeType = occlude;
	lightType = light;
	texcoords = tex;
	this->color = color;
}

BlockTexCoords::BlockTexCoords(){
	
}

BlockTexCoords::BlockTexCoords(uint8 r, uint8 c, uint8 numRows, uint8 numCols) {
	for (int i = 0; i < 6; i++) {
		this->texCoords[i].row = r;
		this->texCoords[i].col = c;
		
	}
}

BlockTexCoords::BlockTexCoords(std::vector<uint8> rc, uint8 numRows, uint8 numCols) {
	assert(rc.size() % 2 == 0);
	assert(rc.size() <= 6*2);

	if (rc.size() == 4) {
		for (int i = 0; i < 6; i++) {
			this->texCoords[i].row = rc[2];
			this->texCoords[i].col = rc[3];
		}

		this->top.row = rc[0];
		this->top.col = rc[1];
	}

}

SpriteTexCoords::SpriteTexCoords(UVType row, UVType col) {
	this->row = row;
	this->col = col;
}

UV SpriteTexCoords::uv0() {
	return UV(0, 0, col, row);
}

UV SpriteTexCoords::uv1() {
	//UV out = spritePos;
	//out.x += width;
	return UV(MAX, 0, col, row);
}

UV SpriteTexCoords::uv2() {
	//UV out = spritePos;
	//out.y += height;
	return UV(0, MAX, col, row);
}

UV SpriteTexCoords::uv3() {
	//UV out = spritePos;
	//out.x += width;
	//out.y += height;
	return UV(MAX, MAX, col, row);;
}

BlockManager* BlockManager::BLOCK_MANAGER_PTR = nullptr;