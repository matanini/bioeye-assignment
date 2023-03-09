#pragma once
#include <fstream> 
#include <ctime>
#include <opencv2/opencv.hpp>

/***
 * Class to handle all the files operations.
 */
class FileHandler {

public:
	FileHandler();
	~FileHandler();

	void close_file();
	void write_to_csv(const int frame_number, const time_t* start_time, const double duration, const bool face_detected);
	void save_image(std::vector<cv::Mat> eyes, const int frame_number) const;
	static std::string parse_time(const tm* time);

private:
	std::ofstream data_file_;
	const char* data_directory_ = "data/";
	const char* img_directory_ = "data/img/";

};
