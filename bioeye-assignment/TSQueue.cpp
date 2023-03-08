#include "TSQueue.h"

template <class T>
TSQueue<T>::TSQueue() : queue() {}

template <class T>
T TSQueue<T>::pop()
{
	std::unique_lock<std::mutex> lock(m);
	//while (queue.empty())
	cv.wait(lock, [&]
		{
			return !queue.empty();
		});
	T data = queue.front();
	queue.pop();
	return data;
}

template <class T>
void TSQueue<T>::push(T data)
{
	std::unique_lock<std::mutex> lock(m);
	queue.push(data);
	lock.unlock();
	cv.notify_all();
}


