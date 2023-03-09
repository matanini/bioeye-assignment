#include "Controller.h"

/**
 * Using Controller class to control the flow.
 * Running the app on 2 threads:
 * 1. main thread - getting video stream from camera and pushing frames to working queue
 * 2. processing thread - retrieving frames from working queue for processing
 */
int main()
{
	// config
	constexpr long long print_fps_interval = 5;
	constexpr int target_fps = 30;

	// declare the controller
	Controller controller(print_fps_interval);

	std::cout << "Initializing..." << std::endl;
	controller.init();

	// main loop
	controller.main_loop(target_fps);

}