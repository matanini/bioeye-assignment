#include "Controller.h"
#include "TSQueue.h"

#include <chrono>

/***
 * Constructor
 * @param interval interval in seconds to print fps to console.
 */
Controller::Controller(const long long interval) :
	fps(0),
	frames_processed_since_last_interval(0),
	eyes_counter(0),
	print_fps_interval(interval),
	finished_capture(false)
{
}

Controller::~Controller()
{
	// close the csv file when destructed
	file_handler.close_file();
}

/***
 * Init of Controller.
 */
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

/***
 * Main loop to start application.
 * @param target_fps target fps.
 */
void Controller::main_loop(const int target_fps)
{
	std::cout << "Starting main loop with target fps:" << target_fps << std::endl;

	// declare lambda function to run in the processor thread
	const auto process_lambda = [this]
	{
		process_queue();
	};
	std::thread frame_processor_thread(process_lambda);;


	cv::Mat frame;
	int total_frames = 0;

	// get starting time for fps calculations
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

		if (fps < target_fps)
		{
			// if target fps is greater than processing fps,
			// push the frame to queue and increment frame number
			queue.push( ++total_frames, frame );
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
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
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
		const std::pair<int,cv::Mat> next_frame = queue.pop();

		// process the frame
		process_frame(next_frame.second, next_frame.first);

		frames_processed_since_last_interval++;

		// stop the loop if finished capture and queue is empty
		if (finished_capture && queue.is_empty())	break;

	}
	std::cout << "Processing thread is finished" << std::endl;
}

/***
 * Processing the frame.
 * @param input_frame src.
 * @param frame_number to maintain order.
 */
void Controller::process_frame(const cv::Mat& input_frame, int frame_number)
{
	// extract 2 eye cv::Mat in a vector 
	start_time_point = std::chrono::system_clock::now();
	processor.process_frame(input_frame, eyes);

	// measure processing performance 
	end_time_point = std::chrono::system_clock::now();
	const std::chrono::duration<double> duration = end_time_point - start_time_point;


	start_time_t = std::chrono::system_clock::to_time_t(start_time_point);

	if (!eyes.empty()) {
		// found eyes in the frame
		// increase the counter
		eyes_counter++;
		// write the data to csv
		file_handler.write_to_csv(frame_number, &start_time_t, duration.count(), true);

		if (eyes_counter % 30 == 0)
			// save the image every 30 instances
			file_handler.save_image(eyes, frame_number);
	}

	else {
		// did not find eyes in frame
		// write the data to csv
		file_handler.write_to_csv(frame_number, &start_time_t, duration.count(), false);
	}
}

/***
 * Calculating the fps and printing to console.
 */
void Controller::calculate_fps()
{
	using namespace std::chrono;

	// recalculate fps
	fps = static_cast<double>(frames_processed_since_last_interval) / print_fps_interval;


	// check if passed {print_fps_interval} seconds, reset the time and print the fps / {print_fps_interval}
	if (duration_cast<seconds>(high_resolution_clock::now() - prev_time) >= seconds{ print_fps_interval }) {
		std::cout << "FPS: " << fps << std::endl;
		prev_time = high_resolution_clock::now();

		// reset processed frames counter
		frames_processed_since_last_interval = 0;
	}
}
