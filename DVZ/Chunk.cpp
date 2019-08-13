#include "Chunk.h"
#include <assert.h>
#include <stdlib.h>
#include "VBO.h"
#include "preamble.glsl"
#include "macrologger.h"

using namespace Voxel;

Chunk::Chunk(int x, int y, int z) : 
	chunk_x(x), 
	chunk_y(y), 
	chunk_z(z), 
	vbo(GL_ARRAY_BUFFER),
	ebo(GL_ELEMENT_ARRAY_BUFFER){

	data = new Block[Chunk::CHUNK_VOLUME];
	this->isMeshValid = false;
}

Chunk::Chunk(int x, int y, int z, Block *data) : 
	chunk_x(x), 
	chunk_y(y), 
	chunk_z(z), 
	data(data), 
	vbo(GL_ARRAY_BUFFER), 
	ebo(GL_ELEMENT_ARRAY_BUFFER){
	this->isMeshValid = false;
}

Chunk::~Chunk() {
	free(data);
	this->vao.dispose();
	this->vbo.dispose();
	this->ebo.dispose();
	LOG_VOXEL("Chunk {%d, %d, %d} deleted", this->chunk_x, this->chunk_y, this->chunk_z);
}

void Chunk::generateTerrain() {
	for (int bz = 0; bz < Chunk::CHUNK_WIDTH_Z; bz++) {
		for (int by = 0; by < Chunk::CHUNK_WIDTH_Y; by++) {
			for (int bx = 0; bx < Chunk::CHUNK_WIDTH_X; bx++) {

				int x = this->chunk_x * CHUNK_WIDTH_X + bx;
				int y = this->chunk_y * CHUNK_WIDTH_Y + by;
				int z = this->chunk_z * CHUNK_WIDTH_Z + bz;

				if (y > 32)
					if (x % 4 == 0 && z % 4 == 0 && y % 4 == 0) {
						this->getBlock(bx, by, bz) = { BlockType::BLOCK_TYPE_STONE };
					}
					else {
						this->getBlock(bx, by, bz) = { BlockType::BLOCK_TYPE_DEFAULT };
					}
				else if ((x * x) % (z * z + 1) > y * y) {
					if(y == 32)
						this->getBlock(bx, by, bz) = { BlockType::BLOCK_TYPE_GRASS };
					else if((x * (z % (y * y + 1))) % 2 == 0)
						this->getBlock(bx, by, bz) = { BlockType::BLOCK_TYPE_DIRT };
					else
						this->getBlock(bx, by, bz) = { BlockType::BLOCK_TYPE_STONE };
					
				}
				else
					this->getBlock(bx, by, bz) = {BlockType::BLOCK_TYPE_DEFAULT };
				//if (x == y && y == z) {
				//	this->getBlock(x, y, z) = { BlockType::BLOCK_TYPE_DIRT };
				//}
				//else {
				//	this->getBlock(x, y, z) = { BlockType::BLOCK_TYPE_DEFAULT };
				//}
			}
		}
	}

	this->isMeshValid = false;
}

void Chunk::generateMesh() {
	this->verticies.clear();
	this->indices.clear();
	for (int z = 0; z < Chunk::CHUNK_WIDTH_Z; z++) {
		for (int y = 0; y < Chunk::CHUNK_WIDTH_Y; y++) {
			for (int x = 0; x < Chunk::CHUNK_WIDTH_X; x++) {
				if (this->getBlock(x, y, z).type == BlockType::BLOCK_TYPE_DEFAULT) {
					continue;
				}
				BlockFaceCullTags render = {true, true, true, true, true, true};

				if (x > 0)
					render.nx = this->getBlock(x - 1, y, z).type == BlockType::BLOCK_TYPE_DEFAULT;
				if (x < Chunk::CHUNK_WIDTH_X - 1)
					render.px = this->getBlock(x + 1, y, z).type == BlockType::BLOCK_TYPE_DEFAULT;
				if (y > 0)
					render.ny = this->getBlock(x, y - 1, z).type == BlockType::BLOCK_TYPE_DEFAULT;
				if (y < Chunk::CHUNK_WIDTH_Y - 1)
					render.py = this->getBlock(x, y + 1, z).type == BlockType::BLOCK_TYPE_DEFAULT;
				if (z > 0)
					render.nz = this->getBlock(x, y, z - 1).type == BlockType::BLOCK_TYPE_DEFAULT;
				if (z < Chunk::CHUNK_WIDTH_Z - 1)
					render.pz = this->getBlock(x, y, z + 1).type == BlockType::BLOCK_TYPE_DEFAULT;

				this->createCube(x, y, z, render, this->getBlock(x, y, z).type);
			}
		}
	}

	this->vao.bind();
	this->vbo.bind();
	this->vbo.bufferData(sizeof(BlockVertex) * this->verticies.size(), this->verticies.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(POSITION_ATTRIB_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(BlockVertex), (void*)0);
	glVertexAttribPointer(NORMAL_ATTRIB_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(BlockVertex), (void*)(1 * sizeof(glm::vec3)));
	glVertexAttribPointer(COLOR_ATTRIB_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(BlockVertex), (void*)(2 * sizeof(glm::vec3)));
	this->vbo.unbind();

	this->ebo.bind();
	this->ebo.bufferData(sizeof(GLuint) * this->indices.size(), this->indices.data(), GL_DYNAMIC_DRAW);
	this->vao.unbind();
	this->ebo.unbind();

	this->indexCount = (int)this->indices.size();
	this->isMeshValid = true;
}

void Chunk::createCube(int x, int y, int z, BlockFaceCullTags render, BlockType type) {
	constexpr float HALF_WIDTH = BLOCK_RENDER_SIZE / 2.0f;
	
	float x_ = x * BLOCK_RENDER_SIZE;
	float y_ = y * BLOCK_RENDER_SIZE;
	float z_ = z * BLOCK_RENDER_SIZE;
	
	glm::vec3 p0(x_ - HALF_WIDTH, y_ - HALF_WIDTH, z_ + HALF_WIDTH);
	glm::vec3 p1(x_ + HALF_WIDTH, y_ - HALF_WIDTH, z_ + HALF_WIDTH);
	glm::vec3 p2(x_ + HALF_WIDTH, y_ + HALF_WIDTH, z_ + HALF_WIDTH);
	glm::vec3 p3(x_ - HALF_WIDTH, y_ + HALF_WIDTH, z_ + HALF_WIDTH);

	glm::vec3 p4(x_ + HALF_WIDTH, y_ - HALF_WIDTH, z_ - HALF_WIDTH);
	glm::vec3 p5(x_ - HALF_WIDTH, y_ - HALF_WIDTH, z_ - HALF_WIDTH);
	glm::vec3 p6(x_ - HALF_WIDTH, y_ + HALF_WIDTH, z_ - HALF_WIDTH);
	glm::vec3 p7(x_ + HALF_WIDTH, y_ + HALF_WIDTH, z_ - HALF_WIDTH);

	glm::vec3 c0, c1, c2, c3;

	if (type == BlockType::BLOCK_TYPE_DIRT) {
		c0 = glm::vec3(.7f, .4f, .4f);
		c1 = glm::vec3(.7f, .4f, .4f);
		c2 = glm::vec3(.7f, .4f, .4f);
		c3 = glm::vec3(.7f, .4f, .4f);
	}
	else if(type == BlockType::BLOCK_TYPE_STONE){
		c0 = glm::vec3(.3f, .3f, .3f);
		c1 = glm::vec3(.3f, .3f, .3f);
		c2 = glm::vec3(.3f, .3f, .3f);
		c3 = glm::vec3(.3f, .3f, .3f);
	}
	else if (type == BlockType::BLOCK_TYPE_GRASS) {
		c0 = glm::vec3(0, 1, 0);
		c1 = glm::vec3(0, 1, 0);
		c2 = glm::vec3(0, 1, 0);
		c3 = glm::vec3(0, 1, 0);
	}
	else if (type == BlockType::BLOCK_TYPE_PURPLE) {
		c0 = glm::vec3(.7f, 0, 1);
		c1 = glm::vec3(.7f, 0, 1);
		c2 = glm::vec3(.7f, 0, 1);
		c3 = glm::vec3(.7f, 0, 1);
	}

	BlockVertex v0, v1, v2, v3;

	/*FRONT*/
	if (render.pz) {
		glm::vec3 n0(0, 0, 1);
		v0 = { p0, n0, c0 };
		v1 = { p1, n0, c1 };
		v2 = { p2, n0, c2 };
		v3 = { p3, n0, c3 };
		createFace(v0, v1, v2, v3);
	}

	/*BACK*/
	if (render.nz) {
		glm::vec3 n1(0, 0, -1);
		v0 = { p4, n1, c0 };
		v1 = { p5, n1, c1 };
		v2 = { p6, n1, c2 };
		v3 = { p7, n1, c3 };
		createFace(v0, v1, v2, v3);
	}

	/*LEFT*/
	if (render.nx) {
		glm::vec3 n2(-1, 0, 0);
		v0 = { p5, n2, c0 };
		v1 = { p0, n2, c1 };
		v2 = { p3, n2, c2 };
		v3 = { p6, n2, c3 };
		createFace(v0, v1, v2, v3);
	}

	/*RIGHT*/
	if (render.px) {
		glm::vec3 n3(1, 0, 0);
		v0 = { p1, n3, c0 };
		v1 = { p4, n3, c1 };
		v2 = { p7, n3, c2 };
		v3 = { p2, n3, c3 };
		createFace(v0, v1, v2, v3);
	}

	/*TOP*/
	if (render.py) {
		glm::vec3 n4(0, 1, 0);
		v0 = { p3, n4, c0 };
		v1 = { p2, n4, c1 };
		v2 = { p7, n4, c2 };
		v3 = { p6, n4, c3 };
		createFace(v0, v1, v2, v3);
	}

	/*BOTTOM*/
	if (render.ny) {
		glm::vec3 n5(0, -1, 0);
		v0 = { p1, n5, c0 };
		v1 = { p0, n5, c1 };
		v2 = { p5, n5, c2 };
		v3 = { p4, n5, c3 };
		createFace(v0, v1, v2, v3);
	}
}

void Chunk::createFace(BlockVertex v0, BlockVertex v1, BlockVertex v2, BlockVertex v3) {
	this->verticies.push_back(v0);
	this->verticies.push_back(v1);
	this->verticies.push_back(v2);
	this->verticies.push_back(v3);

	int lastIndex = this->indices.size() == 0 ? 0 : this->indices.back() + 1;

	this->indices.push_back(lastIndex + 0);
	this->indices.push_back(lastIndex + 1);
	this->indices.push_back(lastIndex + 2);
	this->indices.push_back(lastIndex + 0);
	this->indices.push_back(lastIndex + 2);
	this->indices.push_back(lastIndex + 3);
}

int Chunk::toIndex(int x, int y, int z) {
	return x + Chunk::CHUNK_WIDTH_X * (y + Chunk::CHUNK_WIDTH_Y * z);
}

void Chunk::assertBlockIndex(int x, int y, int z) {
	assert(x >= 0 && x < Chunk::CHUNK_WIDTH_X);
	assert(y >= 0 && y < Chunk::CHUNK_WIDTH_Y);
	assert(z >= 0 && z < Chunk::CHUNK_WIDTH_Z);
}

Block& Chunk::getBlock(int x, int y, int z) {
	this->assertBlockIndex(x, y, z);
	return data[this->toIndex(x, y, z)];
}

void Chunk::setBlock(int x, int y, int z, Block &block) {
	this->assertBlockIndex(x, y, z);

	if (this->data[this->toIndex(x, y, z)] != block) {
		this->data[this->toIndex(x, y, z)] = block;
		this->isMeshValid = false;
	}
}

void Chunk::setBlockData(Block *newData) {
	assert(newData);

	free(this->data);
	this->data = newData;
	this->isMeshValid = false;
}