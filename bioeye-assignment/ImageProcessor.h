#pragma once
#include <opencv2/opencv.hpp>
#include <dlib/image_processing.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>

struct aligned_rectangle_info
{
	cv::Point left_point, right_point;
	double distance, gradient;
};
/***
 * Class to perform the processing operations on an input frame.
 */
class ImageProcessor {
public:
	ImageProcessor();
	void process_frame(const cv::Mat& input_frame, std::vector<cv::Mat>& output_eyes);

private:
	int detect_eyes(const cv::Mat& frame, std::vector<std::array<cv::Point, 2>>& eyes);
	std::array<cv::Point, 4> compute_eye_rect(const cv::Point left_point, const cv::Point right_point, const double distance, const double gradient) const;
	cv::Mat align_and_crop_eye(const cv::Mat& frame, const std::array<cv::Point, 4>& rect_points) const;
	aligned_rectangle_info extract_rect_info(const std::array<cv::Point, 2>& single_eye_coordinates) const;


	dlib::frontal_face_detector detector_;
	dlib::shape_predictor shape_model_;
	std::string landmark_model_path_ = "resources/shape_predictor_5_face_landmarks.dat";
	double scale_factor = 0.4, inflate_rectangle_value = 0.05;
};
