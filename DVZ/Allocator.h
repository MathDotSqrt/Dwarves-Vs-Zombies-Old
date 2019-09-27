#pragma once
#include <assert.h>
#include "Common.h"

namespace Util::Allocator{
	typedef void* voidptr;

	class IAllocator {
	public:
		IAllocator(size_t size, voidptr start);

		virtual ~IAllocator();
	
		virtual voidptr allocate(size_t size, uint8 alignment = 4) = 0;
		virtual void free(voidptr pointer) = 0;

		voidptr getStart() {
			return this->start;
		}

		size_t getAllocSize() {
			return this->buffer_size;
		}

		size_t getUsedMem() {
			return this->used_mem;
		}

		size_t getNumAllocs() {
			return this->used_mem;
		}

	protected:
		voidptr start;
		size_t buffer_size;
		size_t used_mem;
		size_t num_allocs;
	};

	template<typename T>
	T* allocateNew(IAllocator& allocator) {
		return new (allocator.allocate(sizeof(T), __alignof(T))) T;
	}

	template<typename T>
	T* allocNew(IAllocator& allocator, const T& t) {
		return new (allocator.allocate(sizeof(T), __alignof(T))) T(t);
	}

	template<typename T>
	void free(IAllocator& allocator, T *&t) {
		if (t == nullptr) {
			return;
		}

		t->~T();
		allocator.free((voidptr)t);

		//sets t equal to null when user uses free. 
		//This is done to prevent user from attempting to use 
		//T after it has been deallocated
		t = nullptr;
	}

	//todo see if this is actually usefull
	//todo make user keep track of array length
	template<typename T>
	T* allocateArray(IAllocator& allocator, size_t length) {
		assert(length != 0);

		//length of array will be stored here
		uint8 headerSize = sizeof(size_t) / sizeof(T);
		//if does not quite align 
		if (sizeof(size_t) % sizeof(T) > 0) headerSize += 1;
		
		//Allocate extra space to store array length in the bytes before the array 
		//allocator allocates and aligns header + T[] and *p equals to the address of the first element in T[]
		T *p = ((T*) allocator.allocate(sizeof(T) * (length + headerSize), __alignof(T))) + headerSize;

		//Takes p, moves back sizeof(size_t) bytes and dereferences it to store length there
		*(((size_t*)p) - 1) = length;

		//calls constructor for all T in array
		//new (&p) will gaurentee that T will not be allocated randomly in heap
		for (size_t i = 0; i < length; i++) {
			new (&p) T;
		}

		return p;
	}


	template<typename T>
	void freeArray(IAllocator &allocator, T *array) {
		if (array == nullptr) {
			return;
		}

		size_t length = *(((size_t*)array) - 1);

		for (size_t i = 0; i < length; i++) array.~T();

		//Calculate how much extra memory was allocated to store the length before the array 
		uint8 headerSize = sizeof(size_t) / sizeof(T);
		if (sizeof(size_t) % sizeof(T) > 0)
			headerSize += 1;
		allocator.free(array - headerSize);
	}

}

