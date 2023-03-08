#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <opencv2/opencv.hpp>



/***
 * Thread safe unbuffered queue.
 * Using mutex to lock the shared resource and prevent Producer-Consumer problem.
 * Scalable - possible to use with multiple agents.
 */
class TSQueue
{

public:
	TSQueue();

	/***
	* Push frame to the queue.
	* @param frame to push.
	*/
	void push(cv::Mat& frame);

	/***
	* Pop first frame from the queue
	* @return the popped frame
	*/
	cv::Mat pop();

	/***
	 * Check if the queue is empty.
	 */
	bool is_empty() const;

	/***
	 * Return number of frames in queue.
	 */
	int size() const;

private:
	std::queue<cv::Mat> queue;
	std::mutex m;
	std::condition_variable cv;

};
