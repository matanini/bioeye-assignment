#pragma once
#include <fstream> 
#include <ctime>
#include <opencv2/opencv.hpp>

class FileHandler {

public:
	FileHandler();
	~FileHandler();

	void close_file();
	void write_to_csv(const int, const time_t*, const double, const bool);
	void save_image(std::vector<cv::Mat>, const int) const;
	static std::string parse_time(const tm*);

private:
	std::ofstream data_file_;
	const char* data_directory_ = "data/";
	const char* img_directory_ = "data/img/";

};
