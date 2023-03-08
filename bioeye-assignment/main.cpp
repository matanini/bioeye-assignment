#include "Controller.h"

int main()
{
	// declare the controller
	constexpr long long print_fps_interval = 2;
	constexpr int target_fps = 25;

	Controller controller(print_fps_interval);

	std::cout << "Initializing..." << std::endl;
	controller.init();

	
	controller.main_loop(target_fps);

}