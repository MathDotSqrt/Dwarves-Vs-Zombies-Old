#pragma once

#include "Allocator.h"
#include "PoolAllocator.h"
#include <vector>
namespace Util{

template<typename T>
class Recycler {
private:
	Util::Allocator::PoolAllocator pool;
	std::vector<T*> recycled;


public:
	Recycler(size_t alloc_size, Util::Allocator::IAllocator &parent) : pool(sizeof(T), alignof(T), alloc_size, parent){
	
	}
	
	~Recycler() {
		for (T* t : recycled) {
			Util::Allocator::free(this->pool, t);
		}
	}

	T* getNew() {
		if (recycled.size() > 0) {
			T* back = this->recycled.back();
			this->recycled.pop_back();
			return back;
		}

		T* t = Util::Allocator::allocateNew<T>(this->pool);

		return t;
	}

	//todo add a handler class for this to prevent copying of pointer and funny buisness
	void recycle(T *&t) {
		this->recycled.push_back(t);
		t = nullptr;
	}
};

}

