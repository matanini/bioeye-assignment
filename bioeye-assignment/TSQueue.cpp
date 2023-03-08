#include "TSQueue.h"

TSQueue::TSQueue() : queue() {}


cv::Mat TSQueue::pop()
{
	std::cout << std::this_thread::get_id() << ": trying to pop" << std::endl;

	std::unique_lock<std::mutex> lock(m);
	//while (queue.empty())
	cv.wait(lock, [&]
		{
			return !queue.empty();
		});
	cv::Mat data = queue.front();
	std::cout << std::this_thread::get_id() << ": popped" << std::endl;

	queue.pop();
	return data;
}

bool TSQueue::is_empty()
{

	return queue.empty();
}


void TSQueue::push(cv::Mat data)
{
	std::cout << std::this_thread::get_id() << ": trying to push" << std::endl;
	std::unique_lock<std::mutex> lock(m);
	queue.push(data);
	std::cout << std::this_thread::get_id() << ": pushed" << std::endl;

	lock.unlock();
	cv.notify_all();
}


