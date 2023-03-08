#include "TSQueue.h"

TSQueue::TSQueue() : queue() {}


cv::Mat TSQueue::pop()
{
	std::unique_lock<std::mutex> lock(m);
	//while (queue.empty())
	cv.wait(lock, [&]
		{
			return !queue.empty();
		});
	cv::Mat data = queue.front();
	queue.pop();
	return data;
}


void TSQueue::push(cv::Mat data)
{
	std::unique_lock<std::mutex> lock(m);
	queue.push(data);
	lock.unlock();
	cv.notify_all();
}


