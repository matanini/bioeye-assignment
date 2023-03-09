#include "FileHandler.h" 
#include <ctime>
#include <opencv2/opencv.hpp>
#include <filesystem>

void check_dir_and_create(const char* path)
{
	namespace fs = std::filesystem;
	// Check if path dir exists
	if (!fs::is_directory(path) || !fs::exists(path)) { 
		// create path dir
		fs::create_directory(path);
	}
}

FileHandler::FileHandler() {
	// check if needed to create the data + img dir
	check_dir_and_create(data_directory_);
	check_dir_and_create(img_directory_);

	// construct the csv filename with the time now
	const time_t time_now = time(nullptr);
	const tm* t = localtime(&time_now);
	const std::string filename = parse_time(t) + ".csv";

	// create the file and write the columns names in the first line
	data_file_.open(data_directory_ + filename);
	data_file_ << "frame_number,start_time,processing_duration,face_detected" << std::endl;
}
FileHandler::~FileHandler()
{
	// if destructor is called, close the file
	close_file();
}

void FileHandler::write_to_csv(const int frame_number, const time_t* start_time, const double duration, const bool face_detected)
{
	// parsing time_t obj to string
	const std::string start = parse_time(localtime(start_time));
	data_file_ << frame_number << "," << start << "," << duration << "," << face_detected << std::endl;
}

void FileHandler::close_file()
{
	data_file_.close();
}
void FileHandler::save_image(std::vector<cv::Mat> eyes, const int frame_number) const
{
	// for both eyes in the vector
	for (int i = 0; i < 2; i++) {
		std::string filename;
		if (i == 0) {
			// left eye, flip on horizontal axis
			filename = "left_" + std::to_string(frame_number) + ".png";
			flip(eyes[i], eyes[i], 1);
		}
		else {
			filename = "right_" + std::to_string(frame_number) + ".png";
		}
		// save to disk
		imwrite(img_directory_ + filename, eyes[i]);
	}

}

std::string FileHandler::parse_time(const tm* time)
{
	// parsing tm* obj to a meaningful string
	// format is "year-month-day-hour-minute-second"

	char buffer[30];
	strftime(buffer, sizeof(buffer), "%Y-%m-%d-%H-%M-%S", time);
	return std::string(buffer);
}
