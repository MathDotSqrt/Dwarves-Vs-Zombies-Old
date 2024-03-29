#pragma once

#include "Allocator.h"
#include "PoolAllocator.h"

#include <mutex>
#include <vector>

namespace Util{

template<typename T>
class Recycler {
private:
	Util::Allocator::PoolAllocator pool;
	std::vector<T*> allocations;
	std::vector<T*> recycled;

	std::mutex mutex;

	struct RecyclerDeletor {
	public:
		RecyclerDeletor(Recycler *ptr) {
			this->ptr = ptr;
		}

		void operator()(T *t) {
			if (ptr && t) {
				ptr->recycle(t);
			}
		}
	private:
		Recycler *ptr;
	};

public:
	typedef std::shared_ptr<T> SharedHandle;
	//typedef std::unique_ptr<T, RecyclerDeletor> UniqueHandle;

	class UniqueHandle : public std::unique_ptr<T, RecyclerDeletor> {
	public:
		using std::unique_ptr<T, RecyclerDeletor>::unique_ptr;
		
		UniqueHandle() : std::unique_ptr<T, RecyclerDeletor>(nullptr, RecyclerDeletor(nullptr)){
			
		}

	};

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
		assert(t);

		this->allocations.push_back(t);
		return t;
	}

	SharedHandle getSharedNew() {
		T *ptr = getNew();
		return SharedHandle(ptr, RecyclerDeletor(this));
	}

	UniqueHandle getUniqueNew() {
		T *ptr = getNew();
		return UniqueHandle(ptr, RecyclerDeletor(this));
	}

	template<typename ...ARGS>
	T* getNew(ARGS ...args) {
		std::lock_guard<std::mutex> lock(this->mutex);
		if (!recycled.empty()) {
			T* back = this->recycled.back();
			this->recycled.pop_back();
			return back;
		}

		//static int i = 0;

		//printf("%d\n", i);
		//i++;

		T* t = Util::Allocator::allocateNew<T>(this->pool, args...);
		assert(t);
		this->allocations.push_back(t);
		return t;
	}

	template<typename ...ARGS>
	SharedHandle getSharedNew(ARGS ...args) {
		T *ptr = getNew(args...);
		return SharedHandle(ptr, RecyclerDeletor(this));
	}

	template<typename ...ARGS>
	UniqueHandle getUniqueNew(ARGS ...args) {
		T *ptr = getNew(args...);
		return UniqueHandle(ptr, RecyclerDeletor(this));
	}

	UniqueHandle getNullUnique() {
		return UniqueHandle(nullptr, RecyclerDeletor(nullptr));
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
	

};

}

