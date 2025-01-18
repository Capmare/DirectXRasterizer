#include "ThreadPool.h"



ThreadPool::ThreadPool()
{
	size_t numThreads = std::thread::hardware_concurrency();
	m_Pool.reserve(numThreads);
	for (size_t threadIdx{}; threadIdx < numThreads; ++threadIdx)
	{
		m_Pool.emplace_back(std::thread(&ThreadPool::ThreadLoop, this));
	}
}

void ThreadPool::QueueJob(const std::function<void()>& job)
{
	{
		std::unique_lock<std::mutex> lock(m_QueueMutex);
		m_Jobs.push(job);
	}
	m_MutexCondition.notify_one();
}

void ThreadPool::Stop()
{
	{
		std::unique_lock<std::mutex> lock(m_QueueMutex);
		m_ShouldTerminate = true;
	}

	m_MutexCondition.notify_all();
	for (std::thread& ActiveThread : m_Pool)
	{
		ActiveThread.join();
	}

	m_Pool.clear();
}

bool ThreadPool::IsBusy()
{
	std::unique_lock<std::mutex> lock(m_QueueMutex);
	return !m_Jobs.empty();
}

void ThreadPool::ThreadLoop()
{
	while (true)
	{
		std::function<void()> Job;
		{
			std::unique_lock<std::mutex> lock(m_QueueMutex);
			m_MutexCondition.wait(lock, [this] {
				return !m_Jobs.empty() || m_ShouldTerminate; });

			if (m_ShouldTerminate)
			{
				return;
			}
			Job = m_Jobs.front();
			m_Jobs.pop();
		}
		Job();
	}
}
