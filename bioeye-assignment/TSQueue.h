#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <opencv2/opencv.hpp>

class TSQueue
{

public:
	TSQueue();
	void push(cv::Mat data);
	cv::Mat pop();

private:
	std::queue<cv::Mat> queue;
	std::mutex m;
	std::condition_variable cv;

};

