#include "../libThreadPool/ThreadPool.h"

using namespace Thread;

void testThreadPool() {
	ThreadPool thread_pool;
	thread_pool.Start(2);

	std::thread thrd1([&thread_pool] {
		for (int i = 0; i < 10; i++) {
			auto thd_id = std::this_thread::get_id();
			thread_pool.AddTask([thd_id] {
				std::cout << "同步层线程1的线程ID："<< thd_id << std::endl;
				});
		}
	});
	std::thread thrd2([&thread_pool] {
		for (int i = 0; i < 10; i++) {
			auto thd_id = std::this_thread::get_id();
			thread_pool.AddTask([thd_id] {
				std::cout << "同步层线程2的线程ID：" << thd_id << std::endl;
				});
		}
	});

	std::this_thread::sleep_for(std::chrono::seconds(3));
	system("pause");
	thread_pool.Stop();
	thrd1.join();
	thrd2.join();
}

int main(int argc, char** agrv) {
	testThreadPool();
	return 0;
}