#pragma once

#include "Allocator.h"
#include "PoolAllocator.h"
#include <vector>
namespace Util{

template<typename T>
class Recycler {
private:
	Util::Allocator::PoolAllocator pool;
	std::vector<T*> allocations;
	std::vector<T*> recycled;

public:
	Recycler(size_t alloc_size, Util::Allocator::IAllocator &parent) : pool(sizeof(T), alignof(T), alloc_size, parent){
	
	}
	
	~Recycler() {
		for (T* t : allocations) {
			Util::Allocator::free(this->pool, t);
		}
	}

	T* getNew() {
		if (!recycled.empty()) {
			T* back = this->recycled.back();
			this->recycled.pop_back();
			return back;
		}

		T* t = Util::Allocator::allocateNew<T>(this->pool);
		this->allocations.push_back(t);
		return t;
	}

	template<typename ...ARGS>
	T* getNew(ARGS ...args) {
		if (!recycled.empty()) {
			T* back = this->recycled.back();
			this->recycled.pop_back();
			return back;
		}

		T* t = Util::Allocator::allocateNew<T>(this->pool, args...);
		this->allocations.push_back(t);
		return t;
	}

	//todo add a handler class for this to prevent copying of pointer and funny buisness
	void recycle(T *t) {
		this->recycled.push_back(t);
	}

	size_t getNumAllocs() {
		return this->allocations.size();
	}

private:
};

}

