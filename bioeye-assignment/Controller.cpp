#include "Controller.h"
#include "TSQueue.h"

#include <chrono>

Controller::Controller(const long long sec) 
{
	frames_processed_since_last_interval = total_frames = eyes_counter = 0;
	print_fps_interval = sec;
}

Controller::~Controller()
{
	// close the csv file when destructed
	file_handler.close_file();
}

void Controller::init()
{
	// video capture initialization
	cap = cv::VideoCapture(0);
	if (!cap.isOpened()) {
		std::cerr << "Failed to open video stream." << std::endl;
		cv::waitKey(0);
		exit(1);
	}
}

void Controller::main_loop(const int target_fps)
{
	std::cout << "Starting main loop with target fps:" << target_fps << std::endl;

	// declare lambda function to run in the processor thread
	const auto w = [this]
	{
		process_queue();
	};
	std::thread frame_processor_thread(w);


	cv::Mat frame;
	prev_time = std::chrono::high_resolution_clock::now();

	// main loop 
	while (true)
	{
		//calculate fps each frame
		calculate_fps();

		// read the frame
		cap.read(frame);
		if (frame.empty()) {
			std::cerr << "Failed to read frame from video stream." << std::endl;
			break;
		}
		
		// show the frames 
		imshow("Video Stream", frame);

		// if target fps is greater than processing fps
		if (fps < target_fps)
		{
			queue.push(frame);
			frames_processed_since_last_interval++;
			total_frames++;
		}


		// Check for esc key to quit
		if (cv::waitKey(1) == 27) {
			file_handler.close_file();
			break;
		}
	}

	// flag for processing thread
	finished_capture = true;


	// Clean up
	cap.release();
	cv::destroyAllWindows();

	// wait for processor thread to finish
	frame_processor_thread.join();
	
}

/***
 * Function to run in a second thread.
 * Consuming frames from the queue and process them.
 */
void Controller::process_queue()
{
	std::cout << "Processing thread came alive" << std::endl;
	while (true) {
		// pop next frame from the queue
		const cv::Mat next_frame = queue.pop();

		// process the frame
		process_frame(next_frame);

		// stop the loop if finished capture and queue is empty
		if (finished_capture && queue.is_empty())	break;

	}
	std::cout << "Processing thread is finished" << std::endl;
}

/***
 * Processing the frame.
 * @param input_frame src.
 */
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

/***
 * Calculating the fps and printing to console.
 */
void Controller::calculate_fps()
{
	using namespace std::chrono;

	// recalculate fps
	fps = frames_processed_since_last_interval / print_fps_interval;


	// check if passed {print_fps_interval} seconds, reset the time and print the fps / {print_fps_interval}
	if (duration_cast<seconds>(high_resolution_clock::now() - prev_time) >= seconds{ print_fps_interval }) {
		std::cout << "FPS: " << fps << std::endl;
		prev_time = high_resolution_clock::now();

		// reset processed frames counter
		frames_processed_since_last_interval = 0;
	}
}
