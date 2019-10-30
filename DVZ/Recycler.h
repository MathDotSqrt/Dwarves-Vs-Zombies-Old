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

	std::mutex mutex;

public:
	Recycler(size_t alloc_size, Util::Allocator::IAllocator &parent) : pool(sizeof(T), alignof(T), alloc_size, parent){
	
	}
	
	~Recycler() {
		std::lock_guard<std::mutex> lock(this->mutex);
		for (T* t : allocations) {
			Util::Allocator::free(this->pool, t);
		}
	}

	T* getNew() {
		std::lock_guard<std::mutex> lock(this->mutex);
		if (!recycled.empty()) {
			T* back = this->recycled.back();
			this->recycled.pop_back();
			return back;
		}

		T* t = Util::Allocator::allocateNew<T>(this->pool);
		this->allocations.push_back(t);
		return t;
	}

	std::shared_ptr<T> getSharedNew() {
		T *ptr = getNew();
		return std::shared_ptr<T>(ptr, RecyclerDeletor(this));
	}

	std::unique_ptr<T> getUniqueNew() {
		T *ptr = getNew();
		return std::unique_ptr<T>(ptr, RecyclerDeletor(this));
	}

	template<typename ...ARGS>
	T* getNew(ARGS ...args) {
		std::lock_guard<std::mutex> lock(this->mutex);
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
		std::lock_guard<std::mutex> lock(this->mutex);
		this->recycled.push_back(t);
	}

	size_t getNumAllocs() {
		std::lock_guard<std::mutex> lock(this->mutex);
		return this->allocations.size();
	}

private:
	template<typename T>
	struct RecyclerDeletor {
	public:
		RecyclerDeletor(Recycler *ptr) {
			this->ptr = ptr;
		}

		void operator()(T *t) {
			ptr->recycle(t);
		}
	private:
		Recycler *ptr;
	};

};

}

