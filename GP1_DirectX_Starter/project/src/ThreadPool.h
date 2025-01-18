#pragma once
#include "pch.h"

#include <thread>
#include <mutex>
#include <functional>
#include <condition_variable>
#include <vector>
#include <queue>

class ThreadPool
{
public:
	ThreadPool();
	~ThreadPool() = default;

	ThreadPool(const ThreadPool&) = delete;
	ThreadPool(ThreadPool&&) noexcept = delete;
	ThreadPool& operator=(const ThreadPool&) = delete;
	ThreadPool& operator=(ThreadPool&&) noexcept = delete;


	void QueueJob(const std::function<void()>& job);
	void Stop();
	bool IsBusy();
private:
	void ThreadLoop();


	std::queue<std::function<void()>> m_Jobs{};
	std::vector<std::thread> m_Pool{};
	std::mutex m_QueueMutex;
	std::condition_variable m_MutexCondition;
	bool m_ShouldTerminate = false;

};