#include "Controller.h"

int main()
{
	// declare the controller
	constexpr long long print_fps_interval = 5;
	constexpr int target_fps = 15;
	Controller controller(print_fps_interval);

	controller.init();
	controller.main_loop(target_fps);

}