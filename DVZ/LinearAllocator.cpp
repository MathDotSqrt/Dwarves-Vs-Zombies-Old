#include "LinearAllocator.h"
#include <memory>
#include <assert.h>
#include "macrologger.h"


using namespace Util::Allocator;

LinearAllocator::LinearAllocator(size_t size, voidptr start) : IAllocator(size, start) {
	this->currentPtr = this->start;
}

LinearAllocator::LinearAllocator(size_t size, Util::Allocator::IAllocator &parent) : IAllocator(size, parent) {
	this->currentPtr = this->start;
}


LinearAllocator::~LinearAllocator() {
	this->currentPtr = nullptr;
	this->num_allocs = 0;
	this->used_mem = 0;
}

voidptr LinearAllocator::allocate(size_t size, uint8 alignment) {
	assert(size != 0);
	size_t avalible_mem = this->getAvailableMem();


	//std::align only modifies currentPtr and avalible mem(?) if it can fit
	voidptr alignedPointer = std::align(alignment, size, this->currentPtr, avalible_mem);

	//if std::align could not align pointer within buffer size
	if (alignedPointer == nullptr) {
		return nullptr;
	}

	this->currentPtr = (voidptr) (((size_t)this->currentPtr) + size);
	this->used_mem = this->buffer_size - avalible_mem + size;
	this->num_allocs += 1;

	LOG_ALLOC("LINEAR ALLOC");

	return alignedPointer;
}

void LinearAllocator::free(voidptr pointer) {
	LOG_ERROR("Cannot free linear allocator. Must clear entire allocator");
	//assert(0);
}

void LinearAllocator::clear() {
	this->currentPtr = this->start;
	this->used_mem = 0;
	this->num_allocs = 0;
	this->buffer_size;
}
