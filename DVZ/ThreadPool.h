#pragma once
#include <vector>
#include <thread>
#include <future>
#include "ConcurrentQueue.h"
#include "macrologger.h"

namespace Util::Threading {
	//https://github.com/mtrebi/thread-pool/blob/master/include/ThreadPool.h

	class ThreadPool {
	public:
		ThreadPool(int numThreads) : shutdown(false), numThreads(numThreads), workers(std::vector<std::thread>(numThreads)){
			LOG_THREAD("Launching thread pool...");
			for (int i = 0; i < workers.size(); i++) {
				workers[i] = std::thread(WorkerThread(this, i));
				LOG_THREAD("Thread [%d] launched", i);
			}
		}
		~ThreadPool() {
			this->stop();
		}

		template<typename F, typename ...ARGS>
		auto submit(F&& f, ARGS&&... args) -> std::future<decltype(f(args...))> {
			std::function<decltype(f(args...))()> function = std::bind(std::forward<F>(f), std::forward<ARGS>(args)...);

			auto taskptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(function);
			
			std::function<void()> wrapper_function = [taskptr]() {
				(*taskptr)();
			};
			
			tasks.push(std::move(wrapper_function));

			return taskptr->get_future();
		}



		void stop() {
			if (shutdown) {
				return;
			}

			LOG_THREAD("Shutting down thread pools");


			this->shutdown = true;

			for(int i = 0; i < numThreads * 2; i++)		//attempting to flood the thread pool
				this->submit([](){});					//this looks hella cursed

			for (std::thread& worker : this->workers) {
				if (worker.joinable()) {
					worker.join();
					LOG_THREAD("Worker thread joined!");
				}
			}

		}

	private:
		volatile bool shutdown;
		int numThreads;
		std::vector<std::thread> workers;
		Util::BlockingConcurrentQueue<std::function<void()>> tasks;

		class WorkerThread {
		public:
			WorkerThread(ThreadPool *pool, int id) : pool(pool){	//todo add destructor for logging

			}

			void operator()() {
				std::function<void()> function;

				while (!pool->shutdown) {
					pool->tasks.pop(function);
					function();
				}

			}


		private:
			ThreadPool *pool;
		};

		

	};

}

