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

	void push(int frame_number, cv::Mat& frame);
	std::pair<int, cv::Mat> pop();

	[[nodiscard]] bool is_empty() const;
	

private:
	std::queue<std::pair<int,cv::Mat>> queue;
	std::mutex m;
	std::condition_variable cv;

};

