#include "Controller.h"
#include <chrono>

Controller::Controller(const long long sec)
{
	frames_processed_since_last_interval = 0;
	total_frames = 0;
	eyes_counter = 0;
	print_fps_interval = sec;

}

Controller::~Controller()
{
	file_handler.close_file();

}

void Controller::init()
{
	cap = cv::VideoCapture(0);
	if (!cap.isOpened()) {
		std::cerr << "Failed to open video stream." << std::endl;
		cv::waitKey(0);
		exit(1);
	}
}

void Controller::main_loop(const int target_fps)
{
	cv::Mat frame;
	std::map<int, cv::Mat> map;
	bool achieved_target_fps = false;
	while (true)
	{
		calculate_fps();

		cap.read(frame);
		if (frame.empty()) {
			std::cerr << "Failed to read frame from video stream." << std::endl;
			break;
		}
		imshow("Video Stream", frame);

		if (fps < target_fps)
		{
			if (!achieved_target_fps && total_frames > 60)
			{
				// After 2 seconds, if cant achieve target_fps, push to map and process later.
				//map.insert({ total_frames, frame});
				std::cout << "here" << std::endl;
			}
			else
			{
				process_frame(frame);
			}
			frames_processed_since_last_interval++;
		}
		if (fps >= target_fps && total_frames > 60)
		{
			// achieved target fps
			achieved_target_fps = true;
		}

		total_frames++;

		// Check for user input to quit
		if (cv::waitKey(1) == 27) {
			file_handler.close_file();
			break;
		}
	}

	// Clean up
	cap.release();
	cv::destroyAllWindows();

}

void Controller::process_frame(const cv::Mat& input_frame)
{
	// extract 2 eye cv::Mat in a vector and measure performance
	start_time_point = std::chrono::system_clock::now();
	processor.extract_eyes_from_frame(input_frame, eyes);
	end_time_point = std::chrono::system_clock::now();
	const std::chrono::duration<double> duration = end_time_point - start_time_point;

	start_time_t = std::chrono::system_clock::to_time_t(start_time_point);

	if (!eyes.empty()) {
		// found eyes in the frame
		// increase the counter
		eyes_counter++;
		// write the data to csv
		file_handler.write_to_csv(total_frames, &start_time_t, duration.count(), true);

		if (eyes_counter % 30 == 0)
			// save the image every 30 instances
			file_handler.save_image(eyes, total_frames);
	}

	else {
		// did not find eyes in frame
		// write the data to csv
		file_handler.write_to_csv(total_frames, &start_time_t, duration.count(), false);
	}
}

void Controller::calculate_fps()
{
	using namespace std::chrono;

	// static old time interval
	static time_point<high_resolution_clock> old_time = high_resolution_clock::now();

	// recalculate fps
	fps = frames_processed_since_last_interval / print_fps_interval;

	// check if passed {print_fps_interval} seconds, reset the time and print the fps / {print_fps_interval}
	if (duration_cast<seconds>(high_resolution_clock::now() - old_time) >= seconds{ print_fps_interval }) {
		old_time = high_resolution_clock::now();
		std::cout << "FPS: " << fps << std::endl;

		// reset processed frames
		frames_processed_since_last_interval = 0;
	}
}