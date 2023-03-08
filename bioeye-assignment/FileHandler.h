#pragma once
#include <fstream> 
#include <ctime>
#include <opencv2/opencv.hpp>

class FileHandler {
	std::ofstream data_file_;
	std::string data_directory_ = "data/";
	std::string img_directory_ = data_directory_ + "img/";

public:
	FileHandler();
	~FileHandler();

	void close_file();
	void write_to_csv(const int, const time_t*, const double, const bool);
	void save_image(std::vector<cv::Mat>, const int);
	std::string parse_time(const tm*);
};
