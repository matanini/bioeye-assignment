#pragma once
#include "FileHandler.h"
#include "ImageProcessor.h"
#include "TSQueue.h"

#include <opencv2/opencv.hpp>

/***
 * Controller class.
 */
class Controller
{
public:
	Controller(const long long);
	~Controller();

	void init();
	void main_loop(int);
	void calculate_fps();
	void process_frame(const cv::Mat& input_frame);
	void process_queue();

private:
	double fps;
	long long print_fps_interval;
	int frames_processed_since_last_interval, total_frames, eyes_counter;
	bool finished_capture = false;

	cv::VideoCapture cap;
	ImageProcessor processor;
	FileHandler file_handler;
	TSQueue queue;

	time_t start_time_t;
	std::chrono::time_point<std::chrono::system_clock> start_time_point, end_time_point;
	std::chrono::time_point<std::chrono::high_resolution_clock> prev_time;

	std::vector<cv::Mat> eyes;

};

