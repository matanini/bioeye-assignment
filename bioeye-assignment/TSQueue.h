#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>

template <class T>
class TSQueue
{
	TSQueue();
	void push(T data);
	T pop();

private:
	std::queue<T> queue;
	std::mutex m;
	std::condition_variable cv;
};

