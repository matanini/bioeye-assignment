#include "TSQueue.h"

TSQueue::TSQueue() = default;

std::pair<int, cv::Mat> TSQueue::pop()
{
	// wait for lock
	std::unique_lock<std::mutex> lock(m);

	// the thread waits if the queue is empty
	cv.wait(lock, [&]
		{
			return !queue.empty();
		});

	// pop the first frame
	std::pair<int, cv::Mat> frame = queue.front();
	queue.pop();

	return frame;
}

void TSQueue::push(int frame_number, cv::Mat& frame)
{
	// wait for lock 
	std::unique_lock<std::mutex> lock(m);
	// push the frame 
	queue.push({ frame_number,frame });
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

