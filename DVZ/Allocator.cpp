#include "Allocator.h"

using namespace Util::Allocator;

IAllocator::IAllocator(size_t size, voidptr start) : buffer_size(size), start(start){
	assert(size != 0 && start != nullptr);
	this->used_mem = 0;
	this->num_allocs = 0;
}

IAllocator::~IAllocator() {
	assert(this->num_allocs == 0 && this->used_mem == 0);
	this->start = nullptr;
	this->buffer_size = 0;
}