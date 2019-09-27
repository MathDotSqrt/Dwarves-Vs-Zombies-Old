#pragma once
#include "Allocator.h"

namespace Util::Allocator {
	class LinearAllocator : public IAllocator
	{
	public:
		LinearAllocator(size_t size, voidptr start);
		~LinearAllocator();

		voidptr allocate(size_t size, uint8 alignment = 4) override;
		void free(voidptr pointer) override;

		void clear();

	private:
		voidptr currentPtr;
	};

}

