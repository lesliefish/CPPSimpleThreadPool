// ThreadPool.cpp
#include "stdafx.h"
#include "ThreadPool.h"

ThreadPool::ThreadPool()
	:m_isStop(false)
{
}

ThreadPool::~ThreadPool()
{
	m_isStop.store(true);
	m_condition.notify_all();

	for (std::thread& workThread : m_workThreads)
	{
		workThread.detach();
	}
}

/**
* @fn     ThreadPool::start
* @access public
* @brief  start threadpool
* @param  int numThreads : thread number
* @return void
*/
void ThreadPool::start(int numThreads)
{
	m_workThreads.reserve(numThreads);
	for (size_t i = 0; i < numThreads; ++i)
	{
		m_workThreads.emplace_back(
			[this]
		{
			//std::thread::id thread_id = std::this_thread::get_id(); // get thread id, no use here.
			while (!m_isStop)
			{
				std::function<void()> task;
				{
					std::unique_lock<std::mutex> lock{ m_mutex };
					m_condition.wait(lock, [this] { return m_isStop.load() || !m_taskQueue.empty(); });

					if (m_isStop.load() && m_taskQueue.empty())
					{
						return;
					}

					task = std::move(m_taskQueue.front());
					m_taskQueue.pop();
				}
				task();
			}
		}
		);
	}
}

/**
* @fn     ThreadPool::queueSize
* @access public
* @brief  get current tasks size
* @return size_t
*/
size_t ThreadPool::queueSize()
{
	std::unique_lock<std::mutex> lock(m_mutex);
	return m_taskQueue.size();
}