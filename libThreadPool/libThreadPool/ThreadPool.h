#pragma once
#include <list>
#include <thread>
#include <functional>
#include <memory>
#include <atomic>
#include "SyncTaskQueue.h"

namespace Thread {
	constexpr int16_t MaxTaskCount = 100;

	class ThreadPool {
	public:
		using Task = std::function<void()>;
		explicit ThreadPool() : taskQueue_(MaxTaskCount) {
		}
		~ThreadPool() {
			Stop();
		}

		void Start(int num_threads = std::thread::hardware_concurrency()) {
			bExit_ = false;
			for (int i = 0; i < num_threads; ++i) {
				threadGroup_.emplace_back(std::make_shared<std::thread>([this]() {
					while (!bExit_) {
						std::list<Task> list;
						taskQueue_.Take(list);

						for (auto& task : list) {
							if (bExit_) return;
							task();
						}
					}
					}));
			}
		}
		void Stop() {
			std::call_once(flag_, [this] { StopThreadGroup(); });
		}
		void AddTask(Task&& t) {
			taskQueue_.Put(std::forward<Task>(t));
		}
		void AddTask(const Task& t) {
			taskQueue_.Put(t);
		}

	protected:
		void StopThreadGroup() {
			taskQueue_.Stop();
			bExit_ = true;
			for (auto thread : threadGroup_) {
				if (thread && thread->joinable())
					thread->join();
			}
			threadGroup_.clear();
		}
	protected:
		std::list<std::shared_ptr<std::thread>> threadGroup_;
		SyncTaskQueue<Task>		taskQueue_;
		std::atomic_bool		bExit_{ false };
		std::once_flag			flag_;
	};
}