#include "TSQueue.h"

TSQueue::TSQueue() = default;

cv::Mat TSQueue::pop()
{
	// wait for lock
	std::unique_lock<std::mutex> lock(m);

	// the thread waits if the queue is empty
	cv.wait(lock, [&]
		{
			return !queue.empty();
		});

	// pop the first frame
	cv::Mat frame = queue.front();
	queue.pop();

	return frame;
}

void TSQueue::push(cv::Mat& frame)
{
	// wait for lock 
	std::unique_lock<std::mutex> lock(m);
	// push the frame 
	queue.push(frame);
	// unlock the queue
	lock.unlock();
	// wake the consumer
	cv.notify_all();
}

bool TSQueue::is_empty() const
{
	return queue.empty();
}

size_t TSQueue::size() const
{
	return queue.size();
}

