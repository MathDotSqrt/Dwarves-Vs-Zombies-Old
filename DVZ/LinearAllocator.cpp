#include "LinearAllocator.h"
#include <memory>
#include <assert.h>
#include "macrologger.h"


using namespace Util::Allocator;

LinearAllocator::LinearAllocator(size_t size, voidptr start) : IAllocator(size, start) {
	this->currentPtr = start;
}


LinearAllocator::~LinearAllocator() {
	this->currentPtr = nullptr;
}

voidptr LinearAllocator::allocate(size_t size, uint8 alignment) {
	assert(size != 0);

	size_t avalible_mem = this->buffer_size - this->used_mem;

	voidptr alignedPointer = std::align(alignment, size, this->currentPtr, avalible_mem);

	if (alignedPointer == nullptr) {
		return nullptr;
	}

	this->currentPtr = (voidptr) (((size_t)this->currentPtr) + size);
	this->used_mem = this->buffer_size - avalible_mem;
	this->num_allocs += 1;

	return alignedPointer;
}

void LinearAllocator::free(voidptr pointer) {
	LOG_ERROR("Cannot free linear allocator. Must clear entire allocator");
	assert(0);
}

void LinearAllocator::clear() {
	this->currentPtr = this->start;
	this->used_mem = 0;
	this->num_allocs = 0;
}
