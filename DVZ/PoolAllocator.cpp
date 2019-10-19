#include "PoolAllocator.h"
#include <memory>
#include "Timer.h"
using namespace Util::Allocator;

PoolAllocator::PoolAllocator(size_t block_size, uint8 block_alignment, size_t size, voidptr start) 
	: IAllocator(size, start), block_size(block_size), block_alignment(block_alignment) {

	this->init(block_size, block_alignment);
}
PoolAllocator::PoolAllocator(size_t block_size, uint8 block_alignment, size_t size, Util::Allocator::IAllocator &parent) 
	: IAllocator(size, parent), block_size(block_size), block_alignment(block_alignment) {
	this->init(block_size, block_alignment);
}


PoolAllocator::~PoolAllocator() {
	//this->num_allocs = 0;
	//this->used_mem = 0;
}

void PoolAllocator::init(size_t block_size, uint8 block_alignment) {
	//assert block size is bigger than 0
	//assert block size has enough room to store free list pointers
	//assert block size + any alignment for it will be less than allocated size
	assert(block_size != 0);
	assert(block_size >= sizeof(size_t));
	assert((block_size + block_alignment) <= this->buffer_size);

	voidptr alignedAddress = start;
	size_t block_buffer_size = this->buffer_size;

	//will fail if std::align could not align pointer within buffer space
	assert(std::align(block_alignment, block_size, alignedAddress, block_buffer_size));

	this->used_mem = static_cast<uint8*>(alignedAddress) - static_cast<uint8*>(this->start);
	this->num_blocks = block_buffer_size / block_size;

	this->free_list_head = (void**)alignedAddress;
	void **p = this->free_list_head;

	for (size_t i = 0; i < num_blocks - 1; i++) {
		p;
		*p = (voidptr)(reinterpret_cast<uint8*>(p) + block_size);

		p = (void**)*p;
	}

	//tail of free list
	*p = nullptr;
}

voidptr PoolAllocator::allocate(size_t size, uint8 alignment) {
	assert(size == this->block_size && alignment == this->block_alignment);

	if (this->free_list_head == nullptr) {
		return nullptr;
	}

	voidptr p = (void*) this->free_list_head;
	this->free_list_head = (void**)(*this->free_list_head);

	this->used_mem += this->block_size;
	this->num_allocs++;

	return p;
}

void PoolAllocator::free(voidptr pointer) {
	if (pointer == nullptr) {
		return;
	}

	*((void**)pointer) = this->free_list_head;
	this->free_list_head = (void**)pointer;

	this->used_mem -= this->block_size;
	this->num_allocs--;
}
