#pragma once
#include "Allocator.h"
namespace Util::Allocator {
	class PoolAllocator : public IAllocator {
	public:
		PoolAllocator(size_t block_size, uint8 block_alignment, size_t size, voidptr start);
		~PoolAllocator();

		voidptr allocate(size_t size, uint8 alignment = 4) override;
		void free(voidptr pointer) override;

		size_t getMaxNumBlocks() {
			return this->num_blocks;
		}

	private:
		size_t num_blocks;
		size_t block_size;
		uint8 block_alignment;

		void** free_list_head;


		//deleting copy
		PoolAllocator(const PoolAllocator&) = delete;
		PoolAllocator& operator=(const PoolAllocator&) = delete;
	};
}

