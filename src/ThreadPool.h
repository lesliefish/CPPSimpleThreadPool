#pragma once
#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

class ThreadPool
{
	using Task = std::function<void()>;
public:
	// add task func
	template<class Func, class... Args>
	auto addTask(Func&& func, Args&&... args)-> std::future<typename std::result_of<Func(Args...)>::type>
	{
		using funcReturnType = typename std::result_of<Func(Args...)>::type;

		auto task = std::make_shared< std::packaged_task<funcReturnType()> >(std::bind(std::forward<Func>(func), std::forward<Args>(args)...));

		std::future<funcReturnType> res = task->get_future();
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			if (m_isStop.load())
			{
				throw std::runtime_error("addTask on stopped ThreadPool");
			}

			m_taskQueue.emplace([task]() { (*task)(); });
		}
		m_condition.notify_one();
		return res;
	}

public:
	explicit ThreadPool();
	~ThreadPool();

	// start threadpool
	void start(int numThreads);
	// tasks size
	size_t queueSize();

private:
	std::atomic<bool> m_isStop;
	std::mutex m_mutex;
	std::condition_variable m_condition;
	std::vector<std::thread> m_workThreads;
	std::queue<Task> m_taskQueue;
};
#endif  //THREAD_POOL_H

