#pragma once
#include "Allocator.h"
#include <memory>

namespace Util::Allocator {
	template<typename T>
	class AllocatorHandle {
	private:
		using shared_ptr = std::shared_ptr<T>;
	public:
		AllocatorHandle(const IAllocator& allocator) {
			this->allocator = allocator;
		}
		~AllocatorHandle() {
		
		}

		template<typename ...ARGS>
		shared_ptr allocate(ARGS ...args) {
			T* ptr = Util::Allocator::allocateNew<T>(this->pool, args...);
			return shared_ptr(ptr, deleter(allocator));
		}
		
	private:
		struct deleter {
		public:
			deleter(const IAllocator &allocator) {
				this->allocator = allocator;
			}

			void operator()(T *value) {
				Util::Allocator::free<T>(allocator, value);
			}
		private:
			IAllocator &allocator;
		};
		
		IAllocator &allocator;
	};
}


